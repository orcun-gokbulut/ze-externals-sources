//===-- MipsTargetMachine.h - Define TargetMachine for Mips -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Mips specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef MIPSTARGETMACHINE_H
#define MIPSTARGETMACHINE_H

#include "MipsSubtarget.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class formatted_raw_ostream;
class MipsRegisterInfo;

class MipsTargetMachine : public LLVMTargetMachine {
  MipsSubtarget *Subtarget;
  MipsSubtarget DefaultSubtarget;
  MipsSubtarget NoMips16Subtarget;
  MipsSubtarget Mips16Subtarget;

public:
  MipsTargetMachine(const Target &T, StringRef TT, StringRef CPU, StringRef FS,
                    const TargetOptions &Options, Reloc::Model RM,
                    CodeModel::Model CM, CodeGenOpt::Level OL, bool isLittle);

  virtual ~MipsTargetMachine() {}

  void addAnalysisPasses(PassManagerBase &PM) override;

  const MipsInstrInfo *getInstrInfo() const override {
    return getSubtargetImpl()->getInstrInfo();
  }
  const TargetFrameLowering *getFrameLowering() const override {
    return getSubtargetImpl()->getFrameLowering();
  }
  const MipsSubtarget *getSubtargetImpl() const override {
    if (Subtarget)
      return Subtarget;
    return &DefaultSubtarget;
  }
  const InstrItineraryData *getInstrItineraryData() const override {
    return Subtarget->inMips16Mode()
               ? nullptr
               : &getSubtargetImpl()->getInstrItineraryData();
  }
  MipsJITInfo *getJITInfo() override {
    return Subtarget->getJITInfo();
  }
  const MipsRegisterInfo *getRegisterInfo()  const override {
    return getSubtargetImpl()->getRegisterInfo();
  }
  const MipsTargetLowering *getTargetLowering() const override {
    return getSubtargetImpl()->getTargetLowering();
  }
  const DataLayout *getDataLayout() const override {
    return getSubtargetImpl()->getDataLayout();
  }
  const MipsSelectionDAGInfo* getSelectionDAGInfo() const override {
    return getSubtargetImpl()->getSelectionDAGInfo();
  }
  /// \brief Reset the subtarget for the Mips target.
  void resetSubtarget(MachineFunction *MF);

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  bool addCodeEmitter(PassManagerBase &PM, JITCodeEmitter &JCE) override;
};

/// MipsebTargetMachine - Mips32/64 big endian target machine.
///
class MipsebTargetMachine : public MipsTargetMachine {
  virtual void anchor();
public:
  MipsebTargetMachine(const Target &T, StringRef TT,
                      StringRef CPU, StringRef FS, const TargetOptions &Options,
                      Reloc::Model RM, CodeModel::Model CM,
                      CodeGenOpt::Level OL);
};

/// MipselTargetMachine - Mips32/64 little endian target machine.
///
class MipselTargetMachine : public MipsTargetMachine {
  virtual void anchor();
public:
  MipselTargetMachine(const Target &T, StringRef TT,
                      StringRef CPU, StringRef FS, const TargetOptions &Options,
                      Reloc::Model RM, CodeModel::Model CM,
                      CodeGenOpt::Level OL);
};

} // End llvm namespace

#endif
