//===-- PPCInstrInfo.cpp - PowerPC Instruction Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the PowerPC implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "PPCInstrInfo.h"
#include "MCTargetDesc/PPCPredicates.h"
#include "PPC.h"
#include "PPCHazardRecognizers.h"
#include "PPCInstrBuilder.h"
#include "PPCMachineFunctionInfo.h"
#include "PPCTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/LiveIntervalAnalysis.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "ppc-instr-info"

#define GET_INSTRMAP_INFO
#define GET_INSTRINFO_CTOR_DTOR
#include "PPCGenInstrInfo.inc"

static cl::
opt<bool> DisableCTRLoopAnal("disable-ppc-ctrloop-analysis", cl::Hidden,
            cl::desc("Disable analysis for CTR loops"));

static cl::opt<bool> DisableCmpOpt("disable-ppc-cmp-opt",
cl::desc("Disable compare instruction optimization"), cl::Hidden);

static cl::opt<bool> DisableVSXFMAMutate("disable-ppc-vsx-fma-mutation",
cl::desc("Disable VSX FMA instruction mutation"), cl::Hidden);

static cl::opt<bool> VSXSelfCopyCrash("crash-on-ppc-vsx-self-copy",
cl::desc("Causes the backend to crash instead of generating a nop VSX copy"),
cl::Hidden);

// Pin the vtable to this file.
void PPCInstrInfo::anchor() {}

PPCInstrInfo::PPCInstrInfo(PPCSubtarget &STI)
    : PPCGenInstrInfo(PPC::ADJCALLSTACKDOWN, PPC::ADJCALLSTACKUP),
      Subtarget(STI), RI(STI) {}

/// CreateTargetHazardRecognizer - Return the hazard recognizer to use for
/// this target when scheduling the DAG.
ScheduleHazardRecognizer *
PPCInstrInfo::CreateTargetHazardRecognizer(const TargetSubtargetInfo *STI,
                                           const ScheduleDAG *DAG) const {
  unsigned Directive =
      static_cast<const PPCSubtarget *>(STI)->getDarwinDirective();
  if (Directive == PPC::DIR_440 || Directive == PPC::DIR_A2 ||
      Directive == PPC::DIR_E500mc || Directive == PPC::DIR_E5500) {
    const InstrItineraryData *II =
        &static_cast<const PPCSubtarget *>(STI)->getInstrItineraryData();
    return new ScoreboardHazardRecognizer(II, DAG);
  }

  return TargetInstrInfo::CreateTargetHazardRecognizer(STI, DAG);
}

/// CreateTargetPostRAHazardRecognizer - Return the postRA hazard recognizer
/// to use for this target when scheduling the DAG.
ScheduleHazardRecognizer *PPCInstrInfo::CreateTargetPostRAHazardRecognizer(
  const InstrItineraryData *II,
  const ScheduleDAG *DAG) const {
  unsigned Directive =
      DAG->TM.getSubtarget<PPCSubtarget>().getDarwinDirective();

  if (Directive == PPC::DIR_PWR7 || Directive == PPC::DIR_PWR8)
    return new PPCDispatchGroupSBHazardRecognizer(II, DAG);

  // Most subtargets use a PPC970 recognizer.
  if (Directive != PPC::DIR_440 && Directive != PPC::DIR_A2 &&
      Directive != PPC::DIR_E500mc && Directive != PPC::DIR_E5500) {
    assert(DAG->TII && "No InstrInfo?");

    return new PPCHazardRecognizer970(*DAG);
  }

  return new ScoreboardHazardRecognizer(II, DAG);
}


int PPCInstrInfo::getOperandLatency(const InstrItineraryData *ItinData,
                                    const MachineInstr *DefMI, unsigned DefIdx,
                                    const MachineInstr *UseMI,
                                    unsigned UseIdx) const {
  int Latency = PPCGenInstrInfo::getOperandLatency(ItinData, DefMI, DefIdx,
                                                   UseMI, UseIdx);

  const MachineOperand &DefMO = DefMI->getOperand(DefIdx);
  unsigned Reg = DefMO.getReg();

  const TargetRegisterInfo *TRI = &getRegisterInfo();
  bool IsRegCR;
  if (TRI->isVirtualRegister(Reg)) {
    const MachineRegisterInfo *MRI =
      &DefMI->getParent()->getParent()->getRegInfo();
    IsRegCR = MRI->getRegClass(Reg)->hasSuperClassEq(&PPC::CRRCRegClass) ||
              MRI->getRegClass(Reg)->hasSuperClassEq(&PPC::CRBITRCRegClass);
  } else {
    IsRegCR = PPC::CRRCRegClass.contains(Reg) ||
              PPC::CRBITRCRegClass.contains(Reg);
  }

  if (UseMI->isBranch() && IsRegCR) {
    if (Latency < 0)
      Latency = getInstrLatency(ItinData, DefMI);

    // On some cores, there is an additional delay between writing to a condition
    // register, and using it from a branch.
    unsigned Directive = Subtarget.getDarwinDirective();
    switch (Directive) {
    default: break;
    case PPC::DIR_7400:
    case PPC::DIR_750:
    case PPC::DIR_970:
    case PPC::DIR_E5500:
    case PPC::DIR_PWR4:
    case PPC::DIR_PWR5:
    case PPC::DIR_PWR5X:
    case PPC::DIR_PWR6:
    case PPC::DIR_PWR6X:
    case PPC::DIR_PWR7:
    case PPC::DIR_PWR8:
      Latency += 2;
      break;
    }
  }

  return Latency;
}

// Detect 32 -> 64-bit extensions where we may reuse the low sub-register.
bool PPCInstrInfo::isCoalescableExtInstr(const MachineInstr &MI,
                                         unsigned &SrcReg, unsigned &DstReg,
                                         unsigned &SubIdx) const {
  switch (MI.getOpcode()) {
  default: return false;
  case PPC::EXTSW:
  case PPC::EXTSW_32_64:
    SrcReg = MI.getOperand(1).getReg();
    DstReg = MI.getOperand(0).getReg();
    SubIdx = PPC::sub_32;
    return true;
  }
}

unsigned PPCInstrInfo::isLoadFromStackSlot(const MachineInstr *MI,
                                           int &FrameIndex) const {
  // Note: This list must be kept consistent with LoadRegFromStackSlot.
  switch (MI->getOpcode()) {
  default: break;
  case PPC::LD:
  case PPC::LWZ:
  case PPC::LFS:
  case PPC::LFD:
  case PPC::RESTORE_CR:
  case PPC::RESTORE_CRBIT:
  case PPC::LVX:
  case PPC::LXVD2X:
  case PPC::RESTORE_VRSAVE:
    // Check for the operands added by addFrameReference (the immediate is the
    // offset which defaults to 0).
    if (MI->getOperand(1).isImm() && !MI->getOperand(1).getImm() &&
        MI->getOperand(2).isFI()) {
      FrameIndex = MI->getOperand(2).getIndex();
      return MI->getOperand(0).getReg();
    }
    break;
  }
  return 0;
}

unsigned PPCInstrInfo::isStoreToStackSlot(const MachineInstr *MI,
                                          int &FrameIndex) const {
  // Note: This list must be kept consistent with StoreRegToStackSlot.
  switch (MI->getOpcode()) {
  default: break;
  case PPC::STD:
  case PPC::STW:
  case PPC::STFS:
  case PPC::STFD:
  case PPC::SPILL_CR:
  case PPC::SPILL_CRBIT:
  case PPC::STVX:
  case PPC::STXVD2X:
  case PPC::SPILL_VRSAVE:
    // Check for the operands added by addFrameReference (the immediate is the
    // offset which defaults to 0).
    if (MI->getOperand(1).isImm() && !MI->getOperand(1).getImm() &&
        MI->getOperand(2).isFI()) {
      FrameIndex = MI->getOperand(2).getIndex();
      return MI->getOperand(0).getReg();
    }
    break;
  }
  return 0;
}

// commuteInstruction - We can commute rlwimi instructions, but only if the
// rotate amt is zero.  We also have to munge the immediates a bit.
MachineInstr *
PPCInstrInfo::commuteInstruction(MachineInstr *MI, bool NewMI) const {
  MachineFunction &MF = *MI->getParent()->getParent();

  // Normal instructions can be commuted the obvious way.
  if (MI->getOpcode() != PPC::RLWIMI &&
      MI->getOpcode() != PPC::RLWIMIo &&
      MI->getOpcode() != PPC::RLWIMI8 &&
      MI->getOpcode() != PPC::RLWIMI8o)
    return TargetInstrInfo::commuteInstruction(MI, NewMI);

  // Cannot commute if it has a non-zero rotate count.
  if (MI->getOperand(3).getImm() != 0)
    return nullptr;

  // If we have a zero rotate count, we have:
  //   M = mask(MB,ME)
  //   Op0 = (Op1 & ~M) | (Op2 & M)
  // Change this to:
  //   M = mask((ME+1)&31, (MB-1)&31)
  //   Op0 = (Op2 & ~M) | (Op1 & M)

  // Swap op1/op2
  unsigned Reg0 = MI->getOperand(0).getReg();
  unsigned Reg1 = MI->getOperand(1).getReg();
  unsigned Reg2 = MI->getOperand(2).getReg();
  unsigned SubReg1 = MI->getOperand(1).getSubReg();
  unsigned SubReg2 = MI->getOperand(2).getSubReg();
  bool Reg1IsKill = MI->getOperand(1).isKill();
  bool Reg2IsKill = MI->getOperand(2).isKill();
  bool ChangeReg0 = false;
  // If machine instrs are no longer in two-address forms, update
  // destination register as well.
  if (Reg0 == Reg1) {
    // Must be two address instruction!
    assert(MI->getDesc().getOperandConstraint(0, MCOI::TIED_TO) &&
           "Expecting a two-address instruction!");
    assert(MI->getOperand(0).getSubReg() == SubReg1 && "Tied subreg mismatch");
    Reg2IsKill = false;
    ChangeReg0 = true;
  }

  // Masks.
  unsigned MB = MI->getOperand(4).getImm();
  unsigned ME = MI->getOperand(5).getImm();

  if (NewMI) {
    // Create a new instruction.
    unsigned Reg0 = ChangeReg0 ? Reg2 : MI->getOperand(0).getReg();
    bool Reg0IsDead = MI->getOperand(0).isDead();
    return BuildMI(MF, MI->getDebugLoc(), MI->getDesc())
      .addReg(Reg0, RegState::Define | getDeadRegState(Reg0IsDead))
      .addReg(Reg2, getKillRegState(Reg2IsKill))
      .addReg(Reg1, getKillRegState(Reg1IsKill))
      .addImm((ME+1) & 31)
      .addImm((MB-1) & 31);
  }

  if (ChangeReg0) {
    MI->getOperand(0).setReg(Reg2);
    MI->getOperand(0).setSubReg(SubReg2);
  }
  MI->getOperand(2).setReg(Reg1);
  MI->getOperand(1).setReg(Reg2);
  MI->getOperand(2).setSubReg(SubReg1);
  MI->getOperand(1).setSubReg(SubReg2);
  MI->getOperand(2).setIsKill(Reg1IsKill);
  MI->getOperand(1).setIsKill(Reg2IsKill);

  // Swap the mask around.
  MI->getOperand(4).setImm((ME+1) & 31);
  MI->getOperand(5).setImm((MB-1) & 31);
  return MI;
}

bool PPCInstrInfo::findCommutedOpIndices(MachineInstr *MI, unsigned &SrcOpIdx1,
                                         unsigned &SrcOpIdx2) const {
  // For VSX A-Type FMA instructions, it is the first two operands that can be
  // commuted, however, because the non-encoded tied input operand is listed
  // first, the operands to swap are actually the second and third.

  int AltOpc = PPC::getAltVSXFMAOpcode(MI->getOpcode());
  if (AltOpc == -1)
    return TargetInstrInfo::findCommutedOpIndices(MI, SrcOpIdx1, SrcOpIdx2);

  SrcOpIdx1 = 2;
  SrcOpIdx2 = 3;
  return true;
}

void PPCInstrInfo::insertNoop(MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI) const {
  // This function is used for scheduling, and the nop wanted here is the type
  // that terminates dispatch groups on the POWER cores.
  unsigned Directive = Subtarget.getDarwinDirective();
  unsigned Opcode;
  switch (Directive) {
  default:            Opcode = PPC::NOP; break;
  case PPC::DIR_PWR6: Opcode = PPC::NOP_GT_PWR6; break;
  case PPC::DIR_PWR7: Opcode = PPC::NOP_GT_PWR7; break;
  case PPC::DIR_PWR8: Opcode = PPC::NOP_GT_PWR7; break; /* FIXME: Update when P8 InstrScheduling model is ready */
  }

  DebugLoc DL;
  BuildMI(MBB, MI, DL, get(Opcode));
}

// Branch analysis.
// Note: If the condition register is set to CTR or CTR8 then this is a
// BDNZ (imm == 1) or BDZ (imm == 0) branch.
bool PPCInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,MachineBasicBlock *&TBB,
                                 MachineBasicBlock *&FBB,
                                 SmallVectorImpl<MachineOperand> &Cond,
                                 bool AllowModify) const {
  bool isPPC64 = Subtarget.isPPC64();

  // If the block has no terminators, it just falls into the block after it.
  MachineBasicBlock::iterator I = MBB.end();
  if (I == MBB.begin())
    return false;
  --I;
  while (I->isDebugValue()) {
    if (I == MBB.begin())
      return false;
    --I;
  }
  if (!isUnpredicatedTerminator(I))
    return false;

  // Get the last instruction in the block.
  MachineInstr *LastInst = I;

  // If there is only one terminator instruction, process it.
  if (I == MBB.begin() || !isUnpredicatedTerminator(--I)) {
    if (LastInst->getOpcode() == PPC::B) {
      if (!LastInst->getOperand(0).isMBB())
        return true;
      TBB = LastInst->getOperand(0).getMBB();
      return false;
    } else if (LastInst->getOpcode() == PPC::BCC) {
      if (!LastInst->getOperand(2).isMBB())
        return true;
      // Block ends with fall-through condbranch.
      TBB = LastInst->getOperand(2).getMBB();
      Cond.push_back(LastInst->getOperand(0));
      Cond.push_back(LastInst->getOperand(1));
      return false;
    } else if (LastInst->getOpcode() == PPC::BC) {
      if (!LastInst->getOperand(1).isMBB())
        return true;
      // Block ends with fall-through condbranch.
      TBB = LastInst->getOperand(1).getMBB();
      Cond.push_back(MachineOperand::CreateImm(PPC::PRED_BIT_SET));
      Cond.push_back(LastInst->getOperand(0));
      return false;
    } else if (LastInst->getOpcode() == PPC::BCn) {
      if (!LastInst->getOperand(1).isMBB())
        return true;
      // Block ends with fall-through condbranch.
      TBB = LastInst->getOperand(1).getMBB();
      Cond.push_back(MachineOperand::CreateImm(PPC::PRED_BIT_UNSET));
      Cond.push_back(LastInst->getOperand(0));
      return false;
    } else if (LastInst->getOpcode() == PPC::BDNZ8 ||
               LastInst->getOpcode() == PPC::BDNZ) {
      if (!LastInst->getOperand(0).isMBB())
        return true;
      if (DisableCTRLoopAnal)
        return true;
      TBB = LastInst->getOperand(0).getMBB();
      Cond.push_back(MachineOperand::CreateImm(1));
      Cond.push_back(MachineOperand::CreateReg(isPPC64 ? PPC::CTR8 : PPC::CTR,
                                               true));
      return false;
    } else if (LastInst->getOpcode() == PPC::BDZ8 ||
               LastInst->getOpcode() == PPC::BDZ) {
      if (!LastInst->getOperand(0).isMBB())
        return true;
      if (DisableCTRLoopAnal)
        return true;
      TBB = LastInst->getOperand(0).getMBB();
      Cond.push_back(MachineOperand::CreateImm(0));
      Cond.push_back(MachineOperand::CreateReg(isPPC64 ? PPC::CTR8 : PPC::CTR,
                                               true));
      return false;
    }

    // Otherwise, don't know what this is.
    return true;
  }

  // Get the instruction before it if it's a terminator.
  MachineInstr *SecondLastInst = I;

  // If there are three terminators, we don't know what sort of block this is.
  if (SecondLastInst && I != MBB.begin() &&
      isUnpredicatedTerminator(--I))
    return true;

  // If the block ends with PPC::B and PPC:BCC, handle it.
  if (SecondLastInst->getOpcode() == PPC::BCC &&
      LastInst->getOpcode() == PPC::B) {
    if (!SecondLastInst->getOperand(2).isMBB() ||
        !LastInst->getOperand(0).isMBB())
      return true;
    TBB =  SecondLastInst->getOperand(2).getMBB();
    Cond.push_back(SecondLastInst->getOperand(0));
    Cond.push_back(SecondLastInst->getOperand(1));
    FBB = LastInst->getOperand(0).getMBB();
    return false;
  } else if (SecondLastInst->getOpcode() == PPC::BC &&
      LastInst->getOpcode() == PPC::B) {
    if (!SecondLastInst->getOperand(1).isMBB() ||
        !LastInst->getOperand(0).isMBB())
      return true;
    TBB =  SecondLastInst->getOperand(1).getMBB();
    Cond.push_back(MachineOperand::CreateImm(PPC::PRED_BIT_SET));
    Cond.push_back(SecondLastInst->getOperand(0));
    FBB = LastInst->getOperand(0).getMBB();
    return false;
  } else if (SecondLastInst->getOpcode() == PPC::BCn &&
      LastInst->getOpcode() == PPC::B) {
    if (!SecondLastInst->getOperand(1).isMBB() ||
        !LastInst->getOperand(0).isMBB())
      return true;
    TBB =  SecondLastInst->getOperand(1).getMBB();
    Cond.push_back(MachineOperand::CreateImm(PPC::PRED_BIT_UNSET));
    Cond.push_back(SecondLastInst->getOperand(0));
    FBB = LastInst->getOperand(0).getMBB();
    return false;
  } else if ((SecondLastInst->getOpcode() == PPC::BDNZ8 ||
              SecondLastInst->getOpcode() == PPC::BDNZ) &&
      LastInst->getOpcode() == PPC::B) {
    if (!SecondLastInst->getOperand(0).isMBB() ||
        !LastInst->getOperand(0).isMBB())
      return true;
    if (DisableCTRLoopAnal)
      return true;
    TBB = SecondLastInst->getOperand(0).getMBB();
    Cond.push_back(MachineOperand::CreateImm(1));
    Cond.push_back(MachineOperand::CreateReg(isPPC64 ? PPC::CTR8 : PPC::CTR,
                                             true));
    FBB = LastInst->getOperand(0).getMBB();
    return false;
  } else if ((SecondLastInst->getOpcode() == PPC::BDZ8 ||
              SecondLastInst->getOpcode() == PPC::BDZ) &&
      LastInst->getOpcode() == PPC::B) {
    if (!SecondLastInst->getOperand(0).isMBB() ||
        !LastInst->getOperand(0).isMBB())
      return true;
    if (DisableCTRLoopAnal)
      return true;
    TBB = SecondLastInst->getOperand(0).getMBB();
    Cond.push_back(MachineOperand::CreateImm(0));
    Cond.push_back(MachineOperand::CreateReg(isPPC64 ? PPC::CTR8 : PPC::CTR,
                                             true));
    FBB = LastInst->getOperand(0).getMBB();
    return false;
  }

  // If the block ends with two PPC:Bs, handle it.  The second one is not
  // executed, so remove it.
  if (SecondLastInst->getOpcode() == PPC::B &&
      LastInst->getOpcode() == PPC::B) {
    if (!SecondLastInst->getOperand(0).isMBB())
      return true;
    TBB = SecondLastInst->getOperand(0).getMBB();
    I = LastInst;
    if (AllowModify)
      I->eraseFromParent();
    return false;
  }

  // Otherwise, can't handle this.
  return true;
}

unsigned PPCInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator I = MBB.end();
  if (I == MBB.begin()) return 0;
  --I;
  while (I->isDebugValue()) {
    if (I == MBB.begin())
      return 0;
    --I;
  }
  if (I->getOpcode() != PPC::B && I->getOpcode() != PPC::BCC &&
      I->getOpcode() != PPC::BC && I->getOpcode() != PPC::BCn &&
      I->getOpcode() != PPC::BDNZ8 && I->getOpcode() != PPC::BDNZ &&
      I->getOpcode() != PPC::BDZ8  && I->getOpcode() != PPC::BDZ)
    return 0;

  // Remove the branch.
  I->eraseFromParent();

  I = MBB.end();

  if (I == MBB.begin()) return 1;
  --I;
  if (I->getOpcode() != PPC::BCC &&
      I->getOpcode() != PPC::BC && I->getOpcode() != PPC::BCn &&
      I->getOpcode() != PPC::BDNZ8 && I->getOpcode() != PPC::BDNZ &&
      I->getOpcode() != PPC::BDZ8  && I->getOpcode() != PPC::BDZ)
    return 1;

  // Remove the branch.
  I->eraseFromParent();
  return 2;
}

unsigned
PPCInstrInfo::InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                           MachineBasicBlock *FBB,
                           const SmallVectorImpl<MachineOperand> &Cond,
                           DebugLoc DL) const {
  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 2 || Cond.size() == 0) &&
         "PPC branch conditions have two components!");

  bool isPPC64 = Subtarget.isPPC64();

  // One-way branch.
  if (!FBB) {
    if (Cond.empty())   // Unconditional branch
      BuildMI(&MBB, DL, get(PPC::B)).addMBB(TBB);
    else if (Cond[1].getReg() == PPC::CTR || Cond[1].getReg() == PPC::CTR8)
      BuildMI(&MBB, DL, get(Cond[0].getImm() ?
                              (isPPC64 ? PPC::BDNZ8 : PPC::BDNZ) :
                              (isPPC64 ? PPC::BDZ8  : PPC::BDZ))).addMBB(TBB);
    else if (Cond[0].getImm() == PPC::PRED_BIT_SET)
      BuildMI(&MBB, DL, get(PPC::BC)).addOperand(Cond[1]).addMBB(TBB);
    else if (Cond[0].getImm() == PPC::PRED_BIT_UNSET)
      BuildMI(&MBB, DL, get(PPC::BCn)).addOperand(Cond[1]).addMBB(TBB);
    else                // Conditional branch
      BuildMI(&MBB, DL, get(PPC::BCC))
        .addImm(Cond[0].getImm()).addOperand(Cond[1]).addMBB(TBB);
    return 1;
  }

  // Two-way Conditional Branch.
  if (Cond[1].getReg() == PPC::CTR || Cond[1].getReg() == PPC::CTR8)
    BuildMI(&MBB, DL, get(Cond[0].getImm() ?
                            (isPPC64 ? PPC::BDNZ8 : PPC::BDNZ) :
                            (isPPC64 ? PPC::BDZ8  : PPC::BDZ))).addMBB(TBB);
  else if (Cond[0].getImm() == PPC::PRED_BIT_SET)
    BuildMI(&MBB, DL, get(PPC::BC)).addOperand(Cond[1]).addMBB(TBB);
  else if (Cond[0].getImm() == PPC::PRED_BIT_UNSET)
    BuildMI(&MBB, DL, get(PPC::BCn)).addOperand(Cond[1]).addMBB(TBB);
  else
    BuildMI(&MBB, DL, get(PPC::BCC))
      .addImm(Cond[0].getImm()).addOperand(Cond[1]).addMBB(TBB);
  BuildMI(&MBB, DL, get(PPC::B)).addMBB(FBB);
  return 2;
}

// Select analysis.
bool PPCInstrInfo::canInsertSelect(const MachineBasicBlock &MBB,
                const SmallVectorImpl<MachineOperand> &Cond,
                unsigned TrueReg, unsigned FalseReg,
                int &CondCycles, int &TrueCycles, int &FalseCycles) const {
  if (!Subtarget.hasISEL())
    return false;

  if (Cond.size() != 2)
    return false;

  // If this is really a bdnz-like condition, then it cannot be turned into a
  // select.
  if (Cond[1].getReg() == PPC::CTR || Cond[1].getReg() == PPC::CTR8)
    return false;

  // Check register classes.
  const MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterClass *RC =
    RI.getCommonSubClass(MRI.getRegClass(TrueReg), MRI.getRegClass(FalseReg));
  if (!RC)
    return false;

  // isel is for regular integer GPRs only.
  if (!PPC::GPRCRegClass.hasSubClassEq(RC) &&
      !PPC::GPRC_NOR0RegClass.hasSubClassEq(RC) &&
      !PPC::G8RCRegClass.hasSubClassEq(RC) &&
      !PPC::G8RC_NOX0RegClass.hasSubClassEq(RC))
    return false;

  // FIXME: These numbers are for the A2, how well they work for other cores is
  // an open question. On the A2, the isel instruction has a 2-cycle latency
  // but single-cycle throughput. These numbers are used in combination with
  // the MispredictPenalty setting from the active SchedMachineModel.
  CondCycles = 1;
  TrueCycles = 1;
  FalseCycles = 1;

  return true;
}

void PPCInstrInfo::insertSelect(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI, DebugLoc dl,
                                unsigned DestReg,
                                const SmallVectorImpl<MachineOperand> &Cond,
                                unsigned TrueReg, unsigned FalseReg) const {
  assert(Cond.size() == 2 &&
         "PPC branch conditions have two components!");

  assert(Subtarget.hasISEL() &&
         "Cannot insert select on target without ISEL support");

  // Get the register classes.
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterClass *RC =
    RI.getCommonSubClass(MRI.getRegClass(TrueReg), MRI.getRegClass(FalseReg));
  assert(RC && "TrueReg and FalseReg must have overlapping register classes");

  bool Is64Bit = PPC::G8RCRegClass.hasSubClassEq(RC) ||
                 PPC::G8RC_NOX0RegClass.hasSubClassEq(RC);
  assert((Is64Bit ||
          PPC::GPRCRegClass.hasSubClassEq(RC) ||
          PPC::GPRC_NOR0RegClass.hasSubClassEq(RC)) &&
         "isel is for regular integer GPRs only");

  unsigned OpCode = Is64Bit ? PPC::ISEL8 : PPC::ISEL;
  unsigned SelectPred = Cond[0].getImm();

  unsigned SubIdx;
  bool SwapOps;
  switch (SelectPred) {
  default: llvm_unreachable("invalid predicate for isel");
  case PPC::PRED_EQ: SubIdx = PPC::sub_eq; SwapOps = false; break;
  case PPC::PRED_NE: SubIdx = PPC::sub_eq; SwapOps = true; break;
  case PPC::PRED_LT: SubIdx = PPC::sub_lt; SwapOps = false; break;
  case PPC::PRED_GE: SubIdx = PPC::sub_lt; SwapOps = true; break;
  case PPC::PRED_GT: SubIdx = PPC::sub_gt; SwapOps = false; break;
  case PPC::PRED_LE: SubIdx = PPC::sub_gt; SwapOps = true; break;
  case PPC::PRED_UN: SubIdx = PPC::sub_un; SwapOps = false; break;
  case PPC::PRED_NU: SubIdx = PPC::sub_un; SwapOps = true; break;
  case PPC::PRED_BIT_SET:   SubIdx = 0; SwapOps = false; break;
  case PPC::PRED_BIT_UNSET: SubIdx = 0; SwapOps = true; break;
  }

  unsigned FirstReg =  SwapOps ? FalseReg : TrueReg,
           SecondReg = SwapOps ? TrueReg  : FalseReg;

  // The first input register of isel cannot be r0. If it is a member
  // of a register class that can be r0, then copy it first (the
  // register allocator should eliminate the copy).
  if (MRI.getRegClass(FirstReg)->contains(PPC::R0) ||
      MRI.getRegClass(FirstReg)->contains(PPC::X0)) {
    const TargetRegisterClass *FirstRC =
      MRI.getRegClass(FirstReg)->contains(PPC::X0) ?
        &PPC::G8RC_NOX0RegClass : &PPC::GPRC_NOR0RegClass;
    unsigned OldFirstReg = FirstReg;
    FirstReg = MRI.createVirtualRegister(FirstRC);
    BuildMI(MBB, MI, dl, get(TargetOpcode::COPY), FirstReg)
      .addReg(OldFirstReg);
  }

  BuildMI(MBB, MI, dl, get(OpCode), DestReg)
    .addReg(FirstReg).addReg(SecondReg)
    .addReg(Cond[1].getReg(), 0, SubIdx);
}

void PPCInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator I, DebugLoc DL,
                               unsigned DestReg, unsigned SrcReg,
                               bool KillSrc) const {
  // We can end up with self copies and similar things as a result of VSX copy
  // legalization. Promote them here.
  const TargetRegisterInfo *TRI = &getRegisterInfo();
  if (PPC::F8RCRegClass.contains(DestReg) &&
      PPC::VSLRCRegClass.contains(SrcReg)) {
    unsigned SuperReg =
      TRI->getMatchingSuperReg(DestReg, PPC::sub_64, &PPC::VSRCRegClass);

    if (VSXSelfCopyCrash && SrcReg == SuperReg)
      llvm_unreachable("nop VSX copy");

    DestReg = SuperReg;
  } else if (PPC::VRRCRegClass.contains(DestReg) &&
             PPC::VSHRCRegClass.contains(SrcReg)) {
    unsigned SuperReg =
      TRI->getMatchingSuperReg(DestReg, PPC::sub_128, &PPC::VSRCRegClass);

    if (VSXSelfCopyCrash && SrcReg == SuperReg)
      llvm_unreachable("nop VSX copy");

    DestReg = SuperReg;
  } else if (PPC::F8RCRegClass.contains(SrcReg) &&
             PPC::VSLRCRegClass.contains(DestReg)) {
    unsigned SuperReg =
      TRI->getMatchingSuperReg(SrcReg, PPC::sub_64, &PPC::VSRCRegClass);

    if (VSXSelfCopyCrash && DestReg == SuperReg)
      llvm_unreachable("nop VSX copy");

    SrcReg = SuperReg;
  } else if (PPC::VRRCRegClass.contains(SrcReg) &&
             PPC::VSHRCRegClass.contains(DestReg)) {
    unsigned SuperReg =
      TRI->getMatchingSuperReg(SrcReg, PPC::sub_128, &PPC::VSRCRegClass);

    if (VSXSelfCopyCrash && DestReg == SuperReg)
      llvm_unreachable("nop VSX copy");

    SrcReg = SuperReg;
  }

  unsigned Opc;
  if (PPC::GPRCRegClass.contains(DestReg, SrcReg))
    Opc = PPC::OR;
  else if (PPC::G8RCRegClass.contains(DestReg, SrcReg))
    Opc = PPC::OR8;
  else if (PPC::F4RCRegClass.contains(DestReg, SrcReg))
    Opc = PPC::FMR;
  else if (PPC::CRRCRegClass.contains(DestReg, SrcReg))
    Opc = PPC::MCRF;
  else if (PPC::VRRCRegClass.contains(DestReg, SrcReg))
    Opc = PPC::VOR;
  else if (PPC::VSRCRegClass.contains(DestReg, SrcReg))
    // There are two different ways this can be done:
    //   1. xxlor : This has lower latency (on the P7), 2 cycles, but can only
    //      issue in VSU pipeline 0.
    //   2. xmovdp/xmovsp: This has higher latency (on the P7), 6 cycles, but
    //      can go to either pipeline.
    // We'll always use xxlor here, because in practically all cases where
    // copies are generated, they are close enough to some use that the
    // lower-latency form is preferable.
    Opc = PPC::XXLOR;
  else if (PPC::VSFRCRegClass.contains(DestReg, SrcReg))
    Opc = PPC::XXLORf;
  else if (PPC::CRBITRCRegClass.contains(DestReg, SrcReg))
    Opc = PPC::CROR;
  else
    llvm_unreachable("Impossible reg-to-reg copy");

  const MCInstrDesc &MCID = get(Opc);
  if (MCID.getNumOperands() == 3)
    BuildMI(MBB, I, DL, MCID, DestReg)
      .addReg(SrcReg).addReg(SrcReg, getKillRegState(KillSrc));
  else
    BuildMI(MBB, I, DL, MCID, DestReg).addReg(SrcReg, getKillRegState(KillSrc));
}

// This function returns true if a CR spill is necessary and false otherwise.
bool
PPCInstrInfo::StoreRegToStackSlot(MachineFunction &MF,
                                  unsigned SrcReg, bool isKill,
                                  int FrameIdx,
                                  const TargetRegisterClass *RC,
                                  SmallVectorImpl<MachineInstr*> &NewMIs,
                                  bool &NonRI, bool &SpillsVRS) const{
  // Note: If additional store instructions are added here,
  // update isStoreToStackSlot.

  DebugLoc DL;
  if (PPC::GPRCRegClass.hasSubClassEq(RC) ||
      PPC::GPRC_NOR0RegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::STW))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
  } else if (PPC::G8RCRegClass.hasSubClassEq(RC) ||
             PPC::G8RC_NOX0RegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::STD))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
  } else if (PPC::F8RCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::STFD))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
  } else if (PPC::F4RCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::STFS))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
  } else if (PPC::CRRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::SPILL_CR))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
    return true;
  } else if (PPC::CRBITRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::SPILL_CRBIT))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
    return true;
  } else if (PPC::VRRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::STVX))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
    NonRI = true;
  } else if (PPC::VSRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::STXVD2X))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
    NonRI = true;
  } else if (PPC::VSFRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::STXSDX))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
    NonRI = true;
  } else if (PPC::VRSAVERCRegClass.hasSubClassEq(RC)) {
    assert(Subtarget.isDarwin() &&
           "VRSAVE only needs spill/restore on Darwin");
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::SPILL_VRSAVE))
                                       .addReg(SrcReg,
                                               getKillRegState(isKill)),
                                       FrameIdx));
    SpillsVRS = true;
  } else {
    llvm_unreachable("Unknown regclass!");
  }

  return false;
}

void
PPCInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MI,
                                  unsigned SrcReg, bool isKill, int FrameIdx,
                                  const TargetRegisterClass *RC,
                                  const TargetRegisterInfo *TRI) const {
  MachineFunction &MF = *MBB.getParent();
  SmallVector<MachineInstr*, 4> NewMIs;

  PPCFunctionInfo *FuncInfo = MF.getInfo<PPCFunctionInfo>();
  FuncInfo->setHasSpills();

  bool NonRI = false, SpillsVRS = false;
  if (StoreRegToStackSlot(MF, SrcReg, isKill, FrameIdx, RC, NewMIs,
                          NonRI, SpillsVRS))
    FuncInfo->setSpillsCR();

  if (SpillsVRS)
    FuncInfo->setSpillsVRSAVE();

  if (NonRI)
    FuncInfo->setHasNonRISpills();

  for (unsigned i = 0, e = NewMIs.size(); i != e; ++i)
    MBB.insert(MI, NewMIs[i]);

  const MachineFrameInfo &MFI = *MF.getFrameInfo();
  MachineMemOperand *MMO =
    MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(FrameIdx),
                            MachineMemOperand::MOStore,
                            MFI.getObjectSize(FrameIdx),
                            MFI.getObjectAlignment(FrameIdx));
  NewMIs.back()->addMemOperand(MF, MMO);
}

bool
PPCInstrInfo::LoadRegFromStackSlot(MachineFunction &MF, DebugLoc DL,
                                   unsigned DestReg, int FrameIdx,
                                   const TargetRegisterClass *RC,
                                   SmallVectorImpl<MachineInstr*> &NewMIs,
                                   bool &NonRI, bool &SpillsVRS) const{
  // Note: If additional load instructions are added here,
  // update isLoadFromStackSlot.

  if (PPC::GPRCRegClass.hasSubClassEq(RC) ||
      PPC::GPRC_NOR0RegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::LWZ),
                                               DestReg), FrameIdx));
  } else if (PPC::G8RCRegClass.hasSubClassEq(RC) ||
             PPC::G8RC_NOX0RegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::LD), DestReg),
                                       FrameIdx));
  } else if (PPC::F8RCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::LFD), DestReg),
                                       FrameIdx));
  } else if (PPC::F4RCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::LFS), DestReg),
                                       FrameIdx));
  } else if (PPC::CRRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL,
                                               get(PPC::RESTORE_CR), DestReg),
                                       FrameIdx));
    return true;
  } else if (PPC::CRBITRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL,
                                               get(PPC::RESTORE_CRBIT), DestReg),
                                       FrameIdx));
    return true;
  } else if (PPC::VRRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::LVX), DestReg),
                                       FrameIdx));
    NonRI = true;
  } else if (PPC::VSRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::LXVD2X), DestReg),
                                       FrameIdx));
    NonRI = true;
  } else if (PPC::VSFRCRegClass.hasSubClassEq(RC)) {
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL, get(PPC::LXSDX), DestReg),
                                       FrameIdx));
    NonRI = true;
  } else if (PPC::VRSAVERCRegClass.hasSubClassEq(RC)) {
    assert(Subtarget.isDarwin() &&
           "VRSAVE only needs spill/restore on Darwin");
    NewMIs.push_back(addFrameReference(BuildMI(MF, DL,
                                               get(PPC::RESTORE_VRSAVE),
                                               DestReg),
                                       FrameIdx));
    SpillsVRS = true;
  } else {
    llvm_unreachable("Unknown regclass!");
  }

  return false;
}

void
PPCInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MI,
                                   unsigned DestReg, int FrameIdx,
                                   const TargetRegisterClass *RC,
                                   const TargetRegisterInfo *TRI) const {
  MachineFunction &MF = *MBB.getParent();
  SmallVector<MachineInstr*, 4> NewMIs;
  DebugLoc DL;
  if (MI != MBB.end()) DL = MI->getDebugLoc();

  PPCFunctionInfo *FuncInfo = MF.getInfo<PPCFunctionInfo>();
  FuncInfo->setHasSpills();

  bool NonRI = false, SpillsVRS = false;
  if (LoadRegFromStackSlot(MF, DL, DestReg, FrameIdx, RC, NewMIs,
                           NonRI, SpillsVRS))
    FuncInfo->setSpillsCR();

  if (SpillsVRS)
    FuncInfo->setSpillsVRSAVE();

  if (NonRI)
    FuncInfo->setHasNonRISpills();

  for (unsigned i = 0, e = NewMIs.size(); i != e; ++i)
    MBB.insert(MI, NewMIs[i]);

  const MachineFrameInfo &MFI = *MF.getFrameInfo();
  MachineMemOperand *MMO =
    MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(FrameIdx),
                            MachineMemOperand::MOLoad,
                            MFI.getObjectSize(FrameIdx),
                            MFI.getObjectAlignment(FrameIdx));
  NewMIs.back()->addMemOperand(MF, MMO);
}

bool PPCInstrInfo::
ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const {
  assert(Cond.size() == 2 && "Invalid PPC branch opcode!");
  if (Cond[1].getReg() == PPC::CTR8 || Cond[1].getReg() == PPC::CTR)
    Cond[0].setImm(Cond[0].getImm() == 0 ? 1 : 0);
  else
    // Leave the CR# the same, but invert the condition.
    Cond[0].setImm(PPC::InvertPredicate((PPC::Predicate)Cond[0].getImm()));
  return false;
}

bool PPCInstrInfo::FoldImmediate(MachineInstr *UseMI, MachineInstr *DefMI,
                             unsigned Reg, MachineRegisterInfo *MRI) const {
  // For some instructions, it is legal to fold ZERO into the RA register field.
  // A zero immediate should always be loaded with a single li.
  unsigned DefOpc = DefMI->getOpcode();
  if (DefOpc != PPC::LI && DefOpc != PPC::LI8)
    return false;
  if (!DefMI->getOperand(1).isImm())
    return false;
  if (DefMI->getOperand(1).getImm() != 0)
    return false;

  // Note that we cannot here invert the arguments of an isel in order to fold
  // a ZERO into what is presented as the second argument. All we have here
  // is the condition bit, and that might come from a CR-logical bit operation.

  const MCInstrDesc &UseMCID = UseMI->getDesc();

  // Only fold into real machine instructions.
  if (UseMCID.isPseudo())
    return false;

  unsigned UseIdx;
  for (UseIdx = 0; UseIdx < UseMI->getNumOperands(); ++UseIdx)
    if (UseMI->getOperand(UseIdx).isReg() &&
        UseMI->getOperand(UseIdx).getReg() == Reg)
      break;

  assert(UseIdx < UseMI->getNumOperands() && "Cannot find Reg in UseMI");
  assert(UseIdx < UseMCID.getNumOperands() && "No operand description for Reg");

  const MCOperandInfo *UseInfo = &UseMCID.OpInfo[UseIdx];

  // We can fold the zero if this register requires a GPRC_NOR0/G8RC_NOX0
  // register (which might also be specified as a pointer class kind).
  if (UseInfo->isLookupPtrRegClass()) {
    if (UseInfo->RegClass /* Kind */ != 1)
      return false;
  } else {
    if (UseInfo->RegClass != PPC::GPRC_NOR0RegClassID &&
        UseInfo->RegClass != PPC::G8RC_NOX0RegClassID)
      return false;
  }

  // Make sure this is not tied to an output register (or otherwise
  // constrained). This is true for ST?UX registers, for example, which
  // are tied to their output registers.
  if (UseInfo->Constraints != 0)
    return false;

  unsigned ZeroReg;
  if (UseInfo->isLookupPtrRegClass()) {
    bool isPPC64 = Subtarget.isPPC64();
    ZeroReg = isPPC64 ? PPC::ZERO8 : PPC::ZERO;
  } else {
    ZeroReg = UseInfo->RegClass == PPC::G8RC_NOX0RegClassID ?
              PPC::ZERO8 : PPC::ZERO;
  }

  bool DeleteDef = MRI->hasOneNonDBGUse(Reg);
  UseMI->getOperand(UseIdx).setReg(ZeroReg);

  if (DeleteDef)
    DefMI->eraseFromParent();

  return true;
}

static bool MBBDefinesCTR(MachineBasicBlock &MBB) {
  for (MachineBasicBlock::iterator I = MBB.begin(), IE = MBB.end();
       I != IE; ++I)
    if (I->definesRegister(PPC::CTR) || I->definesRegister(PPC::CTR8))
      return true;
  return false;
}

// We should make sure that, if we're going to predicate both sides of a
// condition (a diamond), that both sides don't define the counter register. We
// can predicate counter-decrement-based branches, but while that predicates
// the branching, it does not predicate the counter decrement. If we tried to
// merge the triangle into one predicated block, we'd decrement the counter
// twice.
bool PPCInstrInfo::isProfitableToIfCvt(MachineBasicBlock &TMBB,
                     unsigned NumT, unsigned ExtraT,
                     MachineBasicBlock &FMBB,
                     unsigned NumF, unsigned ExtraF,
                     const BranchProbability &Probability) const {
  return !(MBBDefinesCTR(TMBB) && MBBDefinesCTR(FMBB));
}


bool PPCInstrInfo::isPredicated(const MachineInstr *MI) const {
  // The predicated branches are identified by their type, not really by the
  // explicit presence of a predicate. Furthermore, some of them can be
  // predicated more than once. Because if conversion won't try to predicate
  // any instruction which already claims to be predicated (by returning true
  // here), always return false. In doing so, we let isPredicable() be the
  // final word on whether not the instruction can be (further) predicated.

  return false;
}

bool PPCInstrInfo::isUnpredicatedTerminator(const MachineInstr *MI) const {
  if (!MI->isTerminator())
    return false;

  // Conditional branch is a special case.
  if (MI->isBranch() && !MI->isBarrier())
    return true;

  return !isPredicated(MI);
}

bool PPCInstrInfo::PredicateInstruction(
                     MachineInstr *MI,
                     const SmallVectorImpl<MachineOperand> &Pred) const {
  unsigned OpC = MI->getOpcode();
  if (OpC == PPC::BLR) {
    if (Pred[1].getReg() == PPC::CTR8 || Pred[1].getReg() == PPC::CTR) {
      bool isPPC64 = Subtarget.isPPC64();
      MI->setDesc(get(Pred[0].getImm() ?
                      (isPPC64 ? PPC::BDNZLR8 : PPC::BDNZLR) :
                      (isPPC64 ? PPC::BDZLR8  : PPC::BDZLR)));
    } else if (Pred[0].getImm() == PPC::PRED_BIT_SET) {
      MI->setDesc(get(PPC::BCLR));
      MachineInstrBuilder(*MI->getParent()->getParent(), MI)
        .addReg(Pred[1].getReg());
    } else if (Pred[0].getImm() == PPC::PRED_BIT_UNSET) {
      MI->setDesc(get(PPC::BCLRn));
      MachineInstrBuilder(*MI->getParent()->getParent(), MI)
        .addReg(Pred[1].getReg());
    } else {
      MI->setDesc(get(PPC::BCCLR));
      MachineInstrBuilder(*MI->getParent()->getParent(), MI)
        .addImm(Pred[0].getImm())
        .addReg(Pred[1].getReg());
    }

    return true;
  } else if (OpC == PPC::B) {
    if (Pred[1].getReg() == PPC::CTR8 || Pred[1].getReg() == PPC::CTR) {
      bool isPPC64 = Subtarget.isPPC64();
      MI->setDesc(get(Pred[0].getImm() ?
                      (isPPC64 ? PPC::BDNZ8 : PPC::BDNZ) :
                      (isPPC64 ? PPC::BDZ8  : PPC::BDZ)));
    } else if (Pred[0].getImm() == PPC::PRED_BIT_SET) {
      MachineBasicBlock *MBB = MI->getOperand(0).getMBB();
      MI->RemoveOperand(0);

      MI->setDesc(get(PPC::BC));
      MachineInstrBuilder(*MI->getParent()->getParent(), MI)
        .addReg(Pred[1].getReg())
        .addMBB(MBB);
    } else if (Pred[0].getImm() == PPC::PRED_BIT_UNSET) {
      MachineBasicBlock *MBB = MI->getOperand(0).getMBB();
      MI->RemoveOperand(0);

      MI->setDesc(get(PPC::BCn));
      MachineInstrBuilder(*MI->getParent()->getParent(), MI)
        .addReg(Pred[1].getReg())
        .addMBB(MBB);
    } else {
      MachineBasicBlock *MBB = MI->getOperand(0).getMBB();
      MI->RemoveOperand(0);

      MI->setDesc(get(PPC::BCC));
      MachineInstrBuilder(*MI->getParent()->getParent(), MI)
        .addImm(Pred[0].getImm())
        .addReg(Pred[1].getReg())
        .addMBB(MBB);
    }

    return true;
  } else if (OpC == PPC::BCTR  || OpC == PPC::BCTR8 ||
             OpC == PPC::BCTRL || OpC == PPC::BCTRL8) {
    if (Pred[1].getReg() == PPC::CTR8 || Pred[1].getReg() == PPC::CTR)
      llvm_unreachable("Cannot predicate bctr[l] on the ctr register");

    bool setLR = OpC == PPC::BCTRL || OpC == PPC::BCTRL8;
    bool isPPC64 = Subtarget.isPPC64();

    if (Pred[0].getImm() == PPC::PRED_BIT_SET) {
      MI->setDesc(get(isPPC64 ? (setLR ? PPC::BCCTRL8 : PPC::BCCTR8) :
                                (setLR ? PPC::BCCTRL  : PPC::BCCTR)));
      MachineInstrBuilder(*MI->getParent()->getParent(), MI)
        .addReg(Pred[1].getReg());
      return true;
    } else if (Pred[0].getImm() == PPC::PRED_BIT_UNSET) {
      MI->setDesc(get(isPPC64 ? (setLR ? PPC::BCCTRL8n : PPC::BCCTR8n) :
                                (setLR ? PPC::BCCTRLn  : PPC::BCCTRn)));
      MachineInstrBuilder(*MI->getParent()->getParent(), MI)
        .addReg(Pred[1].getReg());
      return true;
    }

    MI->setDesc(get(isPPC64 ? (setLR ? PPC::BCCCTRL8 : PPC::BCCCTR8) :
                              (setLR ? PPC::BCCCTRL  : PPC::BCCCTR)));
    MachineInstrBuilder(*MI->getParent()->getParent(), MI)
      .addImm(Pred[0].getImm())
      .addReg(Pred[1].getReg());
    return true;
  }

  return false;
}

bool PPCInstrInfo::SubsumesPredicate(
                     const SmallVectorImpl<MachineOperand> &Pred1,
                     const SmallVectorImpl<MachineOperand> &Pred2) const {
  assert(Pred1.size() == 2 && "Invalid PPC first predicate");
  assert(Pred2.size() == 2 && "Invalid PPC second predicate");

  if (Pred1[1].getReg() == PPC::CTR8 || Pred1[1].getReg() == PPC::CTR)
    return false;
  if (Pred2[1].getReg() == PPC::CTR8 || Pred2[1].getReg() == PPC::CTR)
    return false;

  // P1 can only subsume P2 if they test the same condition register.
  if (Pred1[1].getReg() != Pred2[1].getReg())
    return false;

  PPC::Predicate P1 = (PPC::Predicate) Pred1[0].getImm();
  PPC::Predicate P2 = (PPC::Predicate) Pred2[0].getImm();

  if (P1 == P2)
    return true;

  // Does P1 subsume P2, e.g. GE subsumes GT.
  if (P1 == PPC::PRED_LE &&
      (P2 == PPC::PRED_LT || P2 == PPC::PRED_EQ))
    return true;
  if (P1 == PPC::PRED_GE &&
      (P2 == PPC::PRED_GT || P2 == PPC::PRED_EQ))
    return true;

  return false;
}

bool PPCInstrInfo::DefinesPredicate(MachineInstr *MI,
                                    std::vector<MachineOperand> &Pred) const {
  // Note: At the present time, the contents of Pred from this function is
  // unused by IfConversion. This implementation follows ARM by pushing the
  // CR-defining operand. Because the 'DZ' and 'DNZ' count as types of
  // predicate, instructions defining CTR or CTR8 are also included as
  // predicate-defining instructions.

  const TargetRegisterClass *RCs[] =
    { &PPC::CRRCRegClass, &PPC::CRBITRCRegClass,
      &PPC::CTRRCRegClass, &PPC::CTRRC8RegClass };

  bool Found = false;
  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI->getOperand(i);
    for (unsigned c = 0; c < array_lengthof(RCs) && !Found; ++c) {
      const TargetRegisterClass *RC = RCs[c];
      if (MO.isReg()) {
        if (MO.isDef() && RC->contains(MO.getReg())) {
          Pred.push_back(MO);
          Found = true;
        }
      } else if (MO.isRegMask()) {
        for (TargetRegisterClass::iterator I = RC->begin(),
             IE = RC->end(); I != IE; ++I)
          if (MO.clobbersPhysReg(*I)) {
            Pred.push_back(MO);
            Found = true;
          }
      }
    }
  }

  return Found;
}

bool PPCInstrInfo::isPredicable(MachineInstr *MI) const {
  unsigned OpC = MI->getOpcode();
  switch (OpC) {
  default:
    return false;
  case PPC::B:
  case PPC::BLR:
  case PPC::BCTR:
  case PPC::BCTR8:
  case PPC::BCTRL:
  case PPC::BCTRL8:
    return true;
  }
}

bool PPCInstrInfo::analyzeCompare(const MachineInstr *MI,
                                  unsigned &SrcReg, unsigned &SrcReg2,
                                  int &Mask, int &Value) const {
  unsigned Opc = MI->getOpcode();

  switch (Opc) {
  default: return false;
  case PPC::CMPWI:
  case PPC::CMPLWI:
  case PPC::CMPDI:
  case PPC::CMPLDI:
    SrcReg = MI->getOperand(1).getReg();
    SrcReg2 = 0;
    Value = MI->getOperand(2).getImm();
    Mask = 0xFFFF;
    return true;
  case PPC::CMPW:
  case PPC::CMPLW:
  case PPC::CMPD:
  case PPC::CMPLD:
  case PPC::FCMPUS:
  case PPC::FCMPUD:
    SrcReg = MI->getOperand(1).getReg();
    SrcReg2 = MI->getOperand(2).getReg();
    return true;
  }
}

bool PPCInstrInfo::optimizeCompareInstr(MachineInstr *CmpInstr,
                                        unsigned SrcReg, unsigned SrcReg2,
                                        int Mask, int Value,
                                        const MachineRegisterInfo *MRI) const {
  if (DisableCmpOpt)
    return false;

  int OpC = CmpInstr->getOpcode();
  unsigned CRReg = CmpInstr->getOperand(0).getReg();

  // FP record forms set CR1 based on the execption status bits, not a
  // comparison with zero.
  if (OpC == PPC::FCMPUS || OpC == PPC::FCMPUD)
    return false;

  // The record forms set the condition register based on a signed comparison
  // with zero (so says the ISA manual). This is not as straightforward as it
  // seems, however, because this is always a 64-bit comparison on PPC64, even
  // for instructions that are 32-bit in nature (like slw for example).
  // So, on PPC32, for unsigned comparisons, we can use the record forms only
  // for equality checks (as those don't depend on the sign). On PPC64,
  // we are restricted to equality for unsigned 64-bit comparisons and for
  // signed 32-bit comparisons the applicability is more restricted.
  bool isPPC64 = Subtarget.isPPC64();
  bool is32BitSignedCompare   = OpC ==  PPC::CMPWI || OpC == PPC::CMPW;
  bool is32BitUnsignedCompare = OpC == PPC::CMPLWI || OpC == PPC::CMPLW;
  bool is64BitUnsignedCompare = OpC == PPC::CMPLDI || OpC == PPC::CMPLD;

  // Get the unique definition of SrcReg.
  MachineInstr *MI = MRI->getUniqueVRegDef(SrcReg);
  if (!MI) return false;
  int MIOpC = MI->getOpcode();

  bool equalityOnly = false;
  bool noSub = false;
  if (isPPC64) {
    if (is32BitSignedCompare) {
      // We can perform this optimization only if MI is sign-extending.
      if (MIOpC == PPC::SRAW  || MIOpC == PPC::SRAWo ||
          MIOpC == PPC::SRAWI || MIOpC == PPC::SRAWIo ||
          MIOpC == PPC::EXTSB || MIOpC == PPC::EXTSBo ||
          MIOpC == PPC::EXTSH || MIOpC == PPC::EXTSHo ||
          MIOpC == PPC::EXTSW || MIOpC == PPC::EXTSWo) {
        noSub = true;
      } else
        return false;
    } else if (is32BitUnsignedCompare) {
      // We can perform this optimization, equality only, if MI is
      // zero-extending.
      if (MIOpC == PPC::CNTLZW || MIOpC == PPC::CNTLZWo ||
          MIOpC == PPC::SLW    || MIOpC == PPC::SLWo ||
          MIOpC == PPC::SRW    || MIOpC == PPC::SRWo) {
        noSub = true;
        equalityOnly = true;
      } else
        return false;
    } else
      equalityOnly = is64BitUnsignedCompare;
  } else
    equalityOnly = is32BitUnsignedCompare;

  if (equalityOnly) {
    // We need to check the uses of the condition register in order to reject
    // non-equality comparisons.
    for (MachineRegisterInfo::use_instr_iterator I =MRI->use_instr_begin(CRReg),
         IE = MRI->use_instr_end(); I != IE; ++I) {
      MachineInstr *UseMI = &*I;
      if (UseMI->getOpcode() == PPC::BCC) {
        unsigned Pred = UseMI->getOperand(0).getImm();
        if (Pred != PPC::PRED_EQ && Pred != PPC::PRED_NE)
          return false;
      } else if (UseMI->getOpcode() == PPC::ISEL ||
                 UseMI->getOpcode() == PPC::ISEL8) {
        unsigned SubIdx = UseMI->getOperand(3).getSubReg();
        if (SubIdx != PPC::sub_eq)
          return false;
      } else
        return false;
    }
  }

  MachineBasicBlock::iterator I = CmpInstr;

  // Scan forward to find the first use of the compare.
  for (MachineBasicBlock::iterator EL = CmpInstr->getParent()->end();
       I != EL; ++I) {
    bool FoundUse = false;
    for (MachineRegisterInfo::use_instr_iterator J =MRI->use_instr_begin(CRReg),
         JE = MRI->use_instr_end(); J != JE; ++J)
      if (&*J == &*I) {
        FoundUse = true;
        break;
      }

    if (FoundUse)
      break;
  }

  // There are two possible candidates which can be changed to set CR[01].
  // One is MI, the other is a SUB instruction.
  // For CMPrr(r1,r2), we are looking for SUB(r1,r2) or SUB(r2,r1).
  MachineInstr *Sub = nullptr;
  if (SrcReg2 != 0)
    // MI is not a candidate for CMPrr.
    MI = nullptr;
  // FIXME: Conservatively refuse to convert an instruction which isn't in the
  // same BB as the comparison. This is to allow the check below to avoid calls
  // (and other explicit clobbers); instead we should really check for these
  // more explicitly (in at least a few predecessors).
  else if (MI->getParent() != CmpInstr->getParent() || Value != 0) {
    // PPC does not have a record-form SUBri.
    return false;
  }

  // Search for Sub.
  const TargetRegisterInfo *TRI = &getRegisterInfo();
  --I;

  // Get ready to iterate backward from CmpInstr.
  MachineBasicBlock::iterator E = MI,
                              B = CmpInstr->getParent()->begin();

  for (; I != E && !noSub; --I) {
    const MachineInstr &Instr = *I;
    unsigned IOpC = Instr.getOpcode();

    if (&*I != CmpInstr && (
        Instr.modifiesRegister(PPC::CR0, TRI) ||
        Instr.readsRegister(PPC::CR0, TRI)))
      // This instruction modifies or uses the record condition register after
      // the one we want to change. While we could do this transformation, it
      // would likely not be profitable. This transformation removes one
      // instruction, and so even forcing RA to generate one move probably
      // makes it unprofitable.
      return false;

    // Check whether CmpInstr can be made redundant by the current instruction.
    if ((OpC == PPC::CMPW || OpC == PPC::CMPLW ||
         OpC == PPC::CMPD || OpC == PPC::CMPLD) &&
        (IOpC == PPC::SUBF || IOpC == PPC::SUBF8) &&
        ((Instr.getOperand(1).getReg() == SrcReg &&
          Instr.getOperand(2).getReg() == SrcReg2) ||
        (Instr.getOperand(1).getReg() == SrcReg2 &&
         Instr.getOperand(2).getReg() == SrcReg))) {
      Sub = &*I;
      break;
    }

    if (I == B)
      // The 'and' is below the comparison instruction.
      return false;
  }

  // Return false if no candidates exist.
  if (!MI && !Sub)
    return false;

  // The single candidate is called MI.
  if (!MI) MI = Sub;

  int NewOpC = -1;
  MIOpC = MI->getOpcode();
  if (MIOpC == PPC::ANDIo || MIOpC == PPC::ANDIo8)
    NewOpC = MIOpC;
  else {
    NewOpC = PPC::getRecordFormOpcode(MIOpC);
    if (NewOpC == -1 && PPC::getNonRecordFormOpcode(MIOpC) != -1)
      NewOpC = MIOpC;
  }

  // FIXME: On the non-embedded POWER architectures, only some of the record
  // forms are fast, and we should use only the fast ones.

  // The defining instruction has a record form (or is already a record
  // form). It is possible, however, that we'll need to reverse the condition
  // code of the users.
  if (NewOpC == -1)
    return false;

  SmallVector<std::pair<MachineOperand*, PPC::Predicate>, 4> PredsToUpdate;
  SmallVector<std::pair<MachineOperand*, unsigned>, 4> SubRegsToUpdate;

  // If we have SUB(r1, r2) and CMP(r2, r1), the condition code based on CMP
  // needs to be updated to be based on SUB.  Push the condition code
  // operands to OperandsToUpdate.  If it is safe to remove CmpInstr, the
  // condition code of these operands will be modified.
  bool ShouldSwap = false;
  if (Sub) {
    ShouldSwap = SrcReg2 != 0 && Sub->getOperand(1).getReg() == SrcReg2 &&
      Sub->getOperand(2).getReg() == SrcReg;

    // The operands to subf are the opposite of sub, so only in the fixed-point
    // case, invert the order.
    ShouldSwap = !ShouldSwap;
  }

  if (ShouldSwap)
    for (MachineRegisterInfo::use_instr_iterator
         I = MRI->use_instr_begin(CRReg), IE = MRI->use_instr_end();
         I != IE; ++I) {
      MachineInstr *UseMI = &*I;
      if (UseMI->getOpcode() == PPC::BCC) {
        PPC::Predicate Pred = (PPC::Predicate) UseMI->getOperand(0).getImm();
        assert((!equalityOnly ||
                Pred == PPC::PRED_EQ || Pred == PPC::PRED_NE) &&
               "Invalid predicate for equality-only optimization");
        PredsToUpdate.push_back(std::make_pair(&(UseMI->getOperand(0)),
                                PPC::getSwappedPredicate(Pred)));
      } else if (UseMI->getOpcode() == PPC::ISEL ||
                 UseMI->getOpcode() == PPC::ISEL8) {
        unsigned NewSubReg = UseMI->getOperand(3).getSubReg();
        assert((!equalityOnly || NewSubReg == PPC::sub_eq) &&
               "Invalid CR bit for equality-only optimization");

        if (NewSubReg == PPC::sub_lt)
          NewSubReg = PPC::sub_gt;
        else if (NewSubReg == PPC::sub_gt)
          NewSubReg = PPC::sub_lt;

        SubRegsToUpdate.push_back(std::make_pair(&(UseMI->getOperand(3)),
                                                 NewSubReg));
      } else // We need to abort on a user we don't understand.
        return false;
    }

  // Create a new virtual register to hold the value of the CR set by the
  // record-form instruction. If the instruction was not previously in
  // record form, then set the kill flag on the CR.
  CmpInstr->eraseFromParent();

  MachineBasicBlock::iterator MII = MI;
  BuildMI(*MI->getParent(), std::next(MII), MI->getDebugLoc(),
          get(TargetOpcode::COPY), CRReg)
    .addReg(PPC::CR0, MIOpC != NewOpC ? RegState::Kill : 0);

  if (MIOpC != NewOpC) {
    // We need to be careful here: we're replacing one instruction with
    // another, and we need to make sure that we get all of the right
    // implicit uses and defs. On the other hand, the caller may be holding
    // an iterator to this instruction, and so we can't delete it (this is
    // specifically the case if this is the instruction directly after the
    // compare).

    const MCInstrDesc &NewDesc = get(NewOpC);
    MI->setDesc(NewDesc);

    if (NewDesc.ImplicitDefs)
      for (const uint16_t *ImpDefs = NewDesc.getImplicitDefs();
           *ImpDefs; ++ImpDefs)
        if (!MI->definesRegister(*ImpDefs))
          MI->addOperand(*MI->getParent()->getParent(),
                         MachineOperand::CreateReg(*ImpDefs, true, true));
    if (NewDesc.ImplicitUses)
      for (const uint16_t *ImpUses = NewDesc.getImplicitUses();
           *ImpUses; ++ImpUses)
        if (!MI->readsRegister(*ImpUses))
          MI->addOperand(*MI->getParent()->getParent(),
                         MachineOperand::CreateReg(*ImpUses, false, true));
  }

  // Modify the condition code of operands in OperandsToUpdate.
  // Since we have SUB(r1, r2) and CMP(r2, r1), the condition code needs to
  // be changed from r2 > r1 to r1 < r2, from r2 < r1 to r1 > r2, etc.
  for (unsigned i = 0, e = PredsToUpdate.size(); i < e; i++)
    PredsToUpdate[i].first->setImm(PredsToUpdate[i].second);

  for (unsigned i = 0, e = SubRegsToUpdate.size(); i < e; i++)
    SubRegsToUpdate[i].first->setSubReg(SubRegsToUpdate[i].second);

  return true;
}

/// GetInstSize - Return the number of bytes of code the specified
/// instruction may be.  This returns the maximum number of bytes.
///
unsigned PPCInstrInfo::GetInstSizeInBytes(const MachineInstr *MI) const {
  unsigned Opcode = MI->getOpcode();

  if (Opcode == PPC::INLINEASM) {
    const MachineFunction *MF = MI->getParent()->getParent();
    const char *AsmStr = MI->getOperand(0).getSymbolName();
    return getInlineAsmLength(AsmStr, *MF->getTarget().getMCAsmInfo());
  } else {
    const MCInstrDesc &Desc = get(Opcode);
    return Desc.getSize();
  }
}

#undef DEBUG_TYPE
#define DEBUG_TYPE "ppc-vsx-fma-mutate"

namespace {
  // PPCVSXFMAMutate pass - For copies between VSX registers and non-VSX registers
  // (Altivec and scalar floating-point registers), we need to transform the
  // copies into subregister copies with other restrictions.
  struct PPCVSXFMAMutate : public MachineFunctionPass {
    static char ID;
    PPCVSXFMAMutate() : MachineFunctionPass(ID) {
      initializePPCVSXFMAMutatePass(*PassRegistry::getPassRegistry());
    }

    LiveIntervals *LIS;

    const PPCTargetMachine *TM;
    const PPCInstrInfo *TII;

protected:
    bool processBlock(MachineBasicBlock &MBB) {
      bool Changed = false;

      MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
      for (MachineBasicBlock::iterator I = MBB.begin(), IE = MBB.end();
           I != IE; ++I) {
        MachineInstr *MI = I;

        // The default (A-type) VSX FMA form kills the addend (it is taken from
        // the target register, which is then updated to reflect the result of
        // the FMA). If the instruction, however, kills one of the registers
        // used for the product, then we can use the M-form instruction (which
        // will take that value from the to-be-defined register).

        int AltOpc = PPC::getAltVSXFMAOpcode(MI->getOpcode());
        if (AltOpc == -1)
          continue;

        // This pass is run after register coalescing, and so we're looking for
        // a situation like this:
        //   ...
        //   %vreg5<def> = COPY %vreg9; VSLRC:%vreg5,%vreg9
        //   %vreg5<def,tied1> = XSMADDADP %vreg5<tied0>, %vreg17, %vreg16,
        //                         %RM<imp-use>; VSLRC:%vreg5,%vreg17,%vreg16
        //   ...
        //   %vreg9<def,tied1> = XSMADDADP %vreg9<tied0>, %vreg17, %vreg19,
        //                         %RM<imp-use>; VSLRC:%vreg9,%vreg17,%vreg19
        //   ...
        // Where we can eliminate the copy by changing from the A-type to the
        // M-type instruction. Specifically, for this example, this means:
        //   %vreg5<def,tied1> = XSMADDADP %vreg5<tied0>, %vreg17, %vreg16,
        //                         %RM<imp-use>; VSLRC:%vreg5,%vreg17,%vreg16
        // is replaced by:
        //   %vreg16<def,tied1> = XSMADDMDP %vreg16<tied0>, %vreg18, %vreg9,
        //                         %RM<imp-use>; VSLRC:%vreg16,%vreg18,%vreg9
        // and we remove: %vreg5<def> = COPY %vreg9; VSLRC:%vreg5,%vreg9

        SlotIndex FMAIdx = LIS->getInstructionIndex(MI);

        VNInfo *AddendValNo =
          LIS->getInterval(MI->getOperand(1).getReg()).Query(FMAIdx).valueIn();
        MachineInstr *AddendMI = LIS->getInstructionFromIndex(AddendValNo->def);

        // The addend and this instruction must be in the same block.

        if (!AddendMI || AddendMI->getParent() != MI->getParent())
          continue;

        // The addend must be a full copy within the same register class.

        if (!AddendMI->isFullCopy())
          continue;

        unsigned AddendSrcReg = AddendMI->getOperand(1).getReg();
        if (TargetRegisterInfo::isVirtualRegister(AddendSrcReg)) {
          if (MRI.getRegClass(AddendMI->getOperand(0).getReg()) !=
              MRI.getRegClass(AddendSrcReg))
            continue;
        } else {
          // If AddendSrcReg is a physical register, make sure the destination
          // register class contains it.
          if (!MRI.getRegClass(AddendMI->getOperand(0).getReg())
                ->contains(AddendSrcReg))
            continue;
        }

        // In theory, there could be other uses of the addend copy before this
        // fma.  We could deal with this, but that would require additional
        // logic below and I suspect it will not occur in any relevant
        // situations.
        bool OtherUsers = false;
        for (auto J = std::prev(I), JE = MachineBasicBlock::iterator(AddendMI);
             J != JE; --J)
          if (J->readsVirtualRegister(AddendMI->getOperand(0).getReg())) {
            OtherUsers = true;
            break;
          }

        if (OtherUsers)
          continue;

        // Find one of the product operands that is killed by this instruction.

        unsigned KilledProdOp = 0, OtherProdOp = 0;
        if (LIS->getInterval(MI->getOperand(2).getReg())
                     .Query(FMAIdx).isKill()) {
          KilledProdOp = 2;
          OtherProdOp  = 3;
        } else if (LIS->getInterval(MI->getOperand(3).getReg())
                     .Query(FMAIdx).isKill()) {
          KilledProdOp = 3;
          OtherProdOp  = 2;
        }

        // If there are no killed product operands, then this transformation is
        // likely not profitable.
        if (!KilledProdOp)
          continue;

        // In order to replace the addend here with the source of the copy,
        // it must still be live here.
        if (!LIS->getInterval(AddendMI->getOperand(1).getReg()).liveAt(FMAIdx))
          continue;

        // Transform: (O2 * O3) + O1 -> (O2 * O1) + O3.

        unsigned AddReg = AddendMI->getOperand(1).getReg();
        unsigned KilledProdReg = MI->getOperand(KilledProdOp).getReg();
        unsigned OtherProdReg  = MI->getOperand(OtherProdOp).getReg();

        unsigned AddSubReg = AddendMI->getOperand(1).getSubReg();
        unsigned KilledProdSubReg = MI->getOperand(KilledProdOp).getSubReg();
        unsigned OtherProdSubReg  = MI->getOperand(OtherProdOp).getSubReg();

        bool AddRegKill = AddendMI->getOperand(1).isKill();
        bool KilledProdRegKill = MI->getOperand(KilledProdOp).isKill();
        bool OtherProdRegKill  = MI->getOperand(OtherProdOp).isKill();

        bool AddRegUndef = AddendMI->getOperand(1).isUndef();
        bool KilledProdRegUndef = MI->getOperand(KilledProdOp).isUndef();
        bool OtherProdRegUndef  = MI->getOperand(OtherProdOp).isUndef();

        unsigned OldFMAReg = MI->getOperand(0).getReg();

        assert(OldFMAReg == AddendMI->getOperand(0).getReg() &&
               "Addend copy not tied to old FMA output!");

        DEBUG(dbgs() << "VSX FMA Mutation:\n    " << *MI;);

        MI->getOperand(0).setReg(KilledProdReg);
        MI->getOperand(1).setReg(KilledProdReg);
        MI->getOperand(3).setReg(AddReg);
        MI->getOperand(2).setReg(OtherProdReg);

        MI->getOperand(0).setSubReg(KilledProdSubReg);
        MI->getOperand(1).setSubReg(KilledProdSubReg);
        MI->getOperand(3).setSubReg(AddSubReg);
        MI->getOperand(2).setSubReg(OtherProdSubReg);

        MI->getOperand(1).setIsKill(KilledProdRegKill);
        MI->getOperand(3).setIsKill(AddRegKill);
        MI->getOperand(2).setIsKill(OtherProdRegKill);

        MI->getOperand(1).setIsUndef(KilledProdRegUndef);
        MI->getOperand(3).setIsUndef(AddRegUndef);
        MI->getOperand(2).setIsUndef(OtherProdRegUndef);

        MI->setDesc(TII->get(AltOpc));

        DEBUG(dbgs() << " -> " << *MI);

        // The killed product operand was killed here, so we can reuse it now
        // for the result of the fma.

        LiveInterval &FMAInt = LIS->getInterval(OldFMAReg);
        VNInfo *FMAValNo = FMAInt.getVNInfoAt(FMAIdx.getRegSlot());
        for (auto UI = MRI.reg_nodbg_begin(OldFMAReg), UE = MRI.reg_nodbg_end();
             UI != UE;) {
          MachineOperand &UseMO = *UI;
          MachineInstr *UseMI = UseMO.getParent();
          ++UI;

          // Don't replace the result register of the copy we're about to erase.
          if (UseMI == AddendMI)
            continue;

          UseMO.setReg(KilledProdReg);
          UseMO.setSubReg(KilledProdSubReg);
        }

        // Extend the live intervals of the killed product operand to hold the
        // fma result.

        LiveInterval &NewFMAInt = LIS->getInterval(KilledProdReg);
        for (LiveInterval::iterator AI = FMAInt.begin(), AE = FMAInt.end();
             AI != AE; ++AI) {
          // Don't add the segment that corresponds to the original copy.
          if (AI->valno == AddendValNo)
            continue;

          VNInfo *NewFMAValNo =
            NewFMAInt.getNextValue(AI->start,
                                   LIS->getVNInfoAllocator());

          NewFMAInt.addSegment(LiveInterval::Segment(AI->start, AI->end,
                                                     NewFMAValNo));
        }
        DEBUG(dbgs() << "  extended: " << NewFMAInt << '\n');

        FMAInt.removeValNo(FMAValNo);
        DEBUG(dbgs() << "  trimmed:  " << FMAInt << '\n');

        // Remove the (now unused) copy.

        DEBUG(dbgs() << "  removing: " << *AddendMI << '\n');
        LIS->RemoveMachineInstrFromMaps(AddendMI);
        AddendMI->eraseFromParent();

        Changed = true;
      }

      return Changed;
    }

public:
    bool runOnMachineFunction(MachineFunction &MF) override {
      TM = static_cast<const PPCTargetMachine *>(&MF.getTarget());
      // If we don't have VSX then go ahead and return without doing
      // anything.
      if (!TM->getSubtargetImpl()->hasVSX())
        return false;

      LIS = &getAnalysis<LiveIntervals>();

      TII = TM->getInstrInfo();

      bool Changed = false;

      if (DisableVSXFMAMutate)
        return Changed;

      for (MachineFunction::iterator I = MF.begin(); I != MF.end();) {
        MachineBasicBlock &B = *I++;
        if (processBlock(B))
          Changed = true;
      }

      return Changed;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.addRequired<LiveIntervals>();
      AU.addPreserved<LiveIntervals>();
      AU.addRequired<SlotIndexes>();
      AU.addPreserved<SlotIndexes>();
      MachineFunctionPass::getAnalysisUsage(AU);
    }
  };
}

INITIALIZE_PASS_BEGIN(PPCVSXFMAMutate, DEBUG_TYPE,
                      "PowerPC VSX FMA Mutation", false, false)
INITIALIZE_PASS_DEPENDENCY(LiveIntervals)
INITIALIZE_PASS_DEPENDENCY(SlotIndexes)
INITIALIZE_PASS_END(PPCVSXFMAMutate, DEBUG_TYPE,
                    "PowerPC VSX FMA Mutation", false, false)

char &llvm::PPCVSXFMAMutateID = PPCVSXFMAMutate::ID;

char PPCVSXFMAMutate::ID = 0;
FunctionPass*
llvm::createPPCVSXFMAMutatePass() { return new PPCVSXFMAMutate(); }

#undef DEBUG_TYPE
#define DEBUG_TYPE "ppc-vsx-copy"

namespace llvm {
  void initializePPCVSXCopyPass(PassRegistry&);
}

namespace {
  // PPCVSXCopy pass - For copies between VSX registers and non-VSX registers
  // (Altivec and scalar floating-point registers), we need to transform the
  // copies into subregister copies with other restrictions.
  struct PPCVSXCopy : public MachineFunctionPass {
    static char ID;
    PPCVSXCopy() : MachineFunctionPass(ID) {
      initializePPCVSXCopyPass(*PassRegistry::getPassRegistry());
    }

    const PPCTargetMachine *TM;
    const PPCInstrInfo *TII;

    bool IsRegInClass(unsigned Reg, const TargetRegisterClass *RC,
                      MachineRegisterInfo &MRI) {
      if (TargetRegisterInfo::isVirtualRegister(Reg)) {
        return RC->hasSubClassEq(MRI.getRegClass(Reg));
      } else if (RC->contains(Reg)) {
        return true;
      }

      return false;
    }

    bool IsVSReg(unsigned Reg, MachineRegisterInfo &MRI) {
      return IsRegInClass(Reg, &PPC::VSRCRegClass, MRI);
    }

    bool IsVRReg(unsigned Reg, MachineRegisterInfo &MRI) {
      return IsRegInClass(Reg, &PPC::VRRCRegClass, MRI);
    }

    bool IsF8Reg(unsigned Reg, MachineRegisterInfo &MRI) {
      return IsRegInClass(Reg, &PPC::F8RCRegClass, MRI);
    }

protected:
    bool processBlock(MachineBasicBlock &MBB) {
      bool Changed = false;

      MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
      for (MachineBasicBlock::iterator I = MBB.begin(), IE = MBB.end();
           I != IE; ++I) {
        MachineInstr *MI = I;
        if (!MI->isFullCopy())
          continue;

        MachineOperand &DstMO = MI->getOperand(0);
        MachineOperand &SrcMO = MI->getOperand(1);

        if ( IsVSReg(DstMO.getReg(), MRI) &&
            !IsVSReg(SrcMO.getReg(), MRI)) {
          // This is a copy *to* a VSX register from a non-VSX register.
          Changed = true;

          const TargetRegisterClass *SrcRC =
            IsVRReg(SrcMO.getReg(), MRI) ? &PPC::VSHRCRegClass :
                                           &PPC::VSLRCRegClass;
          assert((IsF8Reg(SrcMO.getReg(), MRI) ||
                  IsVRReg(SrcMO.getReg(), MRI)) &&
                 "Unknown source for a VSX copy");

          unsigned NewVReg = MRI.createVirtualRegister(SrcRC);
          BuildMI(MBB, MI, MI->getDebugLoc(),
                  TII->get(TargetOpcode::SUBREG_TO_REG), NewVReg)
            .addImm(1) // add 1, not 0, because there is no implicit clearing
                       // of the high bits.
            .addOperand(SrcMO)
            .addImm(IsVRReg(SrcMO.getReg(), MRI) ? PPC::sub_128 :
                                                   PPC::sub_64);

          // The source of the original copy is now the new virtual register.
          SrcMO.setReg(NewVReg);
        } else if (!IsVSReg(DstMO.getReg(), MRI) &&
                    IsVSReg(SrcMO.getReg(), MRI)) {
          // This is a copy *from* a VSX register to a non-VSX register.
          Changed = true;

          const TargetRegisterClass *DstRC =
            IsVRReg(DstMO.getReg(), MRI) ? &PPC::VSHRCRegClass :
                                           &PPC::VSLRCRegClass;
          assert((IsF8Reg(DstMO.getReg(), MRI) ||
                  IsVRReg(DstMO.getReg(), MRI)) &&
                 "Unknown destination for a VSX copy");

          // Copy the VSX value into a new VSX register of the correct subclass.
          unsigned NewVReg = MRI.createVirtualRegister(DstRC);
          BuildMI(MBB, MI, MI->getDebugLoc(),
                  TII->get(TargetOpcode::COPY), NewVReg)
            .addOperand(SrcMO);

          // Transform the original copy into a subregister extraction copy.
          SrcMO.setReg(NewVReg);
          SrcMO.setSubReg(IsVRReg(DstMO.getReg(), MRI) ? PPC::sub_128 :
                                                         PPC::sub_64);
        }
      }

      return Changed;
    }

public:
    bool runOnMachineFunction(MachineFunction &MF) override {
      TM = static_cast<const PPCTargetMachine *>(&MF.getTarget());
      // If we don't have VSX on the subtarget, don't do anything.
      if (!TM->getSubtargetImpl()->hasVSX())
        return false;
      TII = TM->getInstrInfo();

      bool Changed = false;

      for (MachineFunction::iterator I = MF.begin(); I != MF.end();) {
        MachineBasicBlock &B = *I++;
        if (processBlock(B))
          Changed = true;
      }

      return Changed;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      MachineFunctionPass::getAnalysisUsage(AU);
    }
  };
}

INITIALIZE_PASS(PPCVSXCopy, DEBUG_TYPE,
                "PowerPC VSX Copy Legalization", false, false)

char PPCVSXCopy::ID = 0;
FunctionPass*
llvm::createPPCVSXCopyPass() { return new PPCVSXCopy(); }

#undef DEBUG_TYPE
#define DEBUG_TYPE "ppc-vsx-copy-cleanup"

namespace llvm {
  void initializePPCVSXCopyCleanupPass(PassRegistry&);
}

namespace {
  // PPCVSXCopyCleanup pass - We sometimes end up generating self copies of VSX
  // registers (mostly because the ABI code still places all values into the
  // "traditional" floating-point and vector registers). Remove them here.
  struct PPCVSXCopyCleanup : public MachineFunctionPass {
    static char ID;
    PPCVSXCopyCleanup() : MachineFunctionPass(ID) {
      initializePPCVSXCopyCleanupPass(*PassRegistry::getPassRegistry());
    }

    const PPCTargetMachine *TM;
    const PPCInstrInfo *TII;

protected:
    bool processBlock(MachineBasicBlock &MBB) {
      bool Changed = false;

      SmallVector<MachineInstr *, 4> ToDelete;
      for (MachineBasicBlock::iterator I = MBB.begin(), IE = MBB.end();
           I != IE; ++I) {
        MachineInstr *MI = I;
        if (MI->getOpcode() == PPC::XXLOR &&
            MI->getOperand(0).getReg() == MI->getOperand(1).getReg() &&
            MI->getOperand(0).getReg() == MI->getOperand(2).getReg())
          ToDelete.push_back(MI);
      }

      if (!ToDelete.empty())
        Changed = true;

      for (unsigned i = 0, ie = ToDelete.size(); i != ie; ++i) {
        DEBUG(dbgs() << "Removing VSX self-copy: " << *ToDelete[i]);
        ToDelete[i]->eraseFromParent();
      }

      return Changed;
    }

public:
    bool runOnMachineFunction(MachineFunction &MF) override {
      TM = static_cast<const PPCTargetMachine *>(&MF.getTarget());
      // If we don't have VSX don't bother doing anything here.
      if (!TM->getSubtargetImpl()->hasVSX())
        return false;
      TII = TM->getInstrInfo();

      bool Changed = false;

      for (MachineFunction::iterator I = MF.begin(); I != MF.end();) {
        MachineBasicBlock &B = *I++;
        if (processBlock(B))
          Changed = true;
      }

      return Changed;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      MachineFunctionPass::getAnalysisUsage(AU);
    }
  };
}

INITIALIZE_PASS(PPCVSXCopyCleanup, DEBUG_TYPE,
                "PowerPC VSX Copy Cleanup", false, false)

char PPCVSXCopyCleanup::ID = 0;
FunctionPass*
llvm::createPPCVSXCopyCleanupPass() { return new PPCVSXCopyCleanup(); }

#undef DEBUG_TYPE
#define DEBUG_TYPE "ppc-early-ret"
STATISTIC(NumBCLR, "Number of early conditional returns");
STATISTIC(NumBLR,  "Number of early returns");

namespace llvm {
  void initializePPCEarlyReturnPass(PassRegistry&);
}

namespace {
  // PPCEarlyReturn pass - For simple functions without epilogue code, move
  // returns up, and create conditional returns, to avoid unnecessary
  // branch-to-blr sequences.
  struct PPCEarlyReturn : public MachineFunctionPass {
    static char ID;
    PPCEarlyReturn() : MachineFunctionPass(ID) {
      initializePPCEarlyReturnPass(*PassRegistry::getPassRegistry());
    }

    const PPCTargetMachine *TM;
    const PPCInstrInfo *TII;

protected:
    bool processBlock(MachineBasicBlock &ReturnMBB) {
      bool Changed = false;

      MachineBasicBlock::iterator I = ReturnMBB.begin();
      I = ReturnMBB.SkipPHIsAndLabels(I);

      // The block must be essentially empty except for the blr.
      if (I == ReturnMBB.end() || I->getOpcode() != PPC::BLR ||
          I != ReturnMBB.getLastNonDebugInstr())
        return Changed;

      SmallVector<MachineBasicBlock*, 8> PredToRemove;
      for (MachineBasicBlock::pred_iterator PI = ReturnMBB.pred_begin(),
           PIE = ReturnMBB.pred_end(); PI != PIE; ++PI) {
        bool OtherReference = false, BlockChanged = false;
        for (MachineBasicBlock::iterator J = (*PI)->getLastNonDebugInstr();;) {
          if (J->getOpcode() == PPC::B) {
            if (J->getOperand(0).getMBB() == &ReturnMBB) {
              // This is an unconditional branch to the return. Replace the
              // branch with a blr.
              BuildMI(**PI, J, J->getDebugLoc(), TII->get(PPC::BLR));
              MachineBasicBlock::iterator K = J--;
              K->eraseFromParent();
              BlockChanged = true;
              ++NumBLR;
              continue;
            }
          } else if (J->getOpcode() == PPC::BCC) {
            if (J->getOperand(2).getMBB() == &ReturnMBB) {
              // This is a conditional branch to the return. Replace the branch
              // with a bclr.
              BuildMI(**PI, J, J->getDebugLoc(), TII->get(PPC::BCCLR))
                .addImm(J->getOperand(0).getImm())
                .addReg(J->getOperand(1).getReg());
              MachineBasicBlock::iterator K = J--;
              K->eraseFromParent();
              BlockChanged = true;
              ++NumBCLR;
              continue;
            }
          } else if (J->getOpcode() == PPC::BC || J->getOpcode() == PPC::BCn) {
            if (J->getOperand(1).getMBB() == &ReturnMBB) {
              // This is a conditional branch to the return. Replace the branch
              // with a bclr.
              BuildMI(**PI, J, J->getDebugLoc(),
                      TII->get(J->getOpcode() == PPC::BC ?
                               PPC::BCLR : PPC::BCLRn))
                .addReg(J->getOperand(0).getReg());
              MachineBasicBlock::iterator K = J--;
              K->eraseFromParent();
              BlockChanged = true;
              ++NumBCLR;
              continue;
            }
          } else if (J->isBranch()) {
            if (J->isIndirectBranch()) {
              if (ReturnMBB.hasAddressTaken())
                OtherReference = true;
            } else
              for (unsigned i = 0; i < J->getNumOperands(); ++i)
                if (J->getOperand(i).isMBB() &&
                    J->getOperand(i).getMBB() == &ReturnMBB)
                  OtherReference = true;
          } else if (!J->isTerminator() && !J->isDebugValue())
            break;

          if (J == (*PI)->begin())
            break;

          --J;
        }

        if ((*PI)->canFallThrough() && (*PI)->isLayoutSuccessor(&ReturnMBB))
          OtherReference = true;

        // Predecessors are stored in a vector and can't be removed here.
        if (!OtherReference && BlockChanged) {
          PredToRemove.push_back(*PI);
        }

        if (BlockChanged)
          Changed = true;
      }

      for (unsigned i = 0, ie = PredToRemove.size(); i != ie; ++i)
        PredToRemove[i]->removeSuccessor(&ReturnMBB);

      if (Changed && !ReturnMBB.hasAddressTaken()) {
        // We now might be able to merge this blr-only block into its
        // by-layout predecessor.
        if (ReturnMBB.pred_size() == 1 &&
            (*ReturnMBB.pred_begin())->isLayoutSuccessor(&ReturnMBB)) {
          // Move the blr into the preceding block.
          MachineBasicBlock &PrevMBB = **ReturnMBB.pred_begin();
          PrevMBB.splice(PrevMBB.end(), &ReturnMBB, I);
          PrevMBB.removeSuccessor(&ReturnMBB);
        }

        if (ReturnMBB.pred_empty())
          ReturnMBB.eraseFromParent();
      }

      return Changed;
    }

public:
    bool runOnMachineFunction(MachineFunction &MF) override {
      TM = static_cast<const PPCTargetMachine *>(&MF.getTarget());
      TII = TM->getInstrInfo();

      bool Changed = false;

      // If the function does not have at least two blocks, then there is
      // nothing to do.
      if (MF.size() < 2)
        return Changed;

      for (MachineFunction::iterator I = MF.begin(); I != MF.end();) {
        MachineBasicBlock &B = *I++;
        if (processBlock(B))
          Changed = true;
      }

      return Changed;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      MachineFunctionPass::getAnalysisUsage(AU);
    }
  };
}

INITIALIZE_PASS(PPCEarlyReturn, DEBUG_TYPE,
                "PowerPC Early-Return Creation", false, false)

char PPCEarlyReturn::ID = 0;
FunctionPass*
llvm::createPPCEarlyReturnPass() { return new PPCEarlyReturn(); }
