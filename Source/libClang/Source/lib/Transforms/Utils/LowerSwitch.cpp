//===- LowerSwitch.cpp - Eliminate Switch instructions --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// The LowerSwitch transformation rewrites switch instructions with a sequence
// of branches, which allows targets to get away with not implementing the
// switch instruction until it is convenient.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/CFG.h"
#include "llvm/Pass.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include <algorithm>
using namespace llvm;

#define DEBUG_TYPE "lower-switch"

namespace {
  /// LowerSwitch Pass - Replace all SwitchInst instructions with chained branch
  /// instructions.
  class LowerSwitch : public FunctionPass {
  public:
    static char ID; // Pass identification, replacement for typeid
    LowerSwitch() : FunctionPass(ID) {
      initializeLowerSwitchPass(*PassRegistry::getPassRegistry());
    } 

    bool runOnFunction(Function &F) override;

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      // This is a cluster of orthogonal Transforms
      AU.addPreserved<UnifyFunctionExitNodes>();
      AU.addPreserved("mem2reg");
      AU.addPreservedID(LowerInvokePassID);
    }

    struct CaseRange {
      Constant* Low;
      Constant* High;
      BasicBlock* BB;

      CaseRange(Constant *low = nullptr, Constant *high = nullptr,
                BasicBlock *bb = nullptr) :
        Low(low), High(high), BB(bb) { }
    };

    typedef std::vector<CaseRange> CaseVector;
    typedef std::vector<CaseRange>::iterator CaseItr;
  private:
    void processSwitchInst(SwitchInst *SI);

    BasicBlock *switchConvert(CaseItr Begin, CaseItr End,
                              ConstantInt *LowerBound, ConstantInt *UpperBound,
                              Value *Val, BasicBlock *Predecessor,
                              BasicBlock *OrigBlock, BasicBlock *Default);
    BasicBlock *newLeafBlock(CaseRange &Leaf, Value *Val, BasicBlock *OrigBlock,
                             BasicBlock *Default);
    unsigned Clusterify(CaseVector &Cases, SwitchInst *SI);
  };

  /// The comparison function for sorting the switch case values in the vector.
  /// WARNING: Case ranges should be disjoint!
  struct CaseCmp {
    bool operator () (const LowerSwitch::CaseRange& C1,
                      const LowerSwitch::CaseRange& C2) {

      const ConstantInt* CI1 = cast<const ConstantInt>(C1.Low);
      const ConstantInt* CI2 = cast<const ConstantInt>(C2.High);
      return CI1->getValue().slt(CI2->getValue());
    }
  };
}

char LowerSwitch::ID = 0;
INITIALIZE_PASS(LowerSwitch, "lowerswitch",
                "Lower SwitchInst's to branches", false, false)

// Publicly exposed interface to pass...
char &llvm::LowerSwitchID = LowerSwitch::ID;
// createLowerSwitchPass - Interface to this file...
FunctionPass *llvm::createLowerSwitchPass() {
  return new LowerSwitch();
}

bool LowerSwitch::runOnFunction(Function &F) {
  bool Changed = false;

  for (Function::iterator I = F.begin(), E = F.end(); I != E; ) {
    BasicBlock *Cur = I++; // Advance over block so we don't traverse new blocks

    if (SwitchInst *SI = dyn_cast<SwitchInst>(Cur->getTerminator())) {
      Changed = true;
      processSwitchInst(SI);
    }
  }

  return Changed;
}

// operator<< - Used for debugging purposes.
//
static raw_ostream& operator<<(raw_ostream &O,
                               const LowerSwitch::CaseVector &C)
    LLVM_ATTRIBUTE_USED;
static raw_ostream& operator<<(raw_ostream &O,
                               const LowerSwitch::CaseVector &C) {
  O << "[";

  for (LowerSwitch::CaseVector::const_iterator B = C.begin(),
         E = C.end(); B != E; ) {
    O << *B->Low << " -" << *B->High;
    if (++B != E) O << ", ";
  }

  return O << "]";
}

static void fixPhis(BasicBlock *Succ,
                    BasicBlock *OrigBlock,
                    BasicBlock *NewNode) {
  for (BasicBlock::iterator I = Succ->begin(),
                            E = Succ->getFirstNonPHI();
       I != E; ++I) {
    PHINode *PN = cast<PHINode>(I);

    for (unsigned I = 0, E = PN->getNumIncomingValues(); I != E; ++I) {
      if (PN->getIncomingBlock(I) == OrigBlock)
        PN->setIncomingBlock(I, NewNode);
    }
  }
}

// switchConvert - Convert the switch statement into a binary lookup of
// the case values. The function recursively builds this tree.
// LowerBound and UpperBound are used to keep track of the bounds for Val
// that have already been checked by a block emitted by one of the previous
// calls to switchConvert in the call stack.
BasicBlock *LowerSwitch::switchConvert(CaseItr Begin, CaseItr End,
                                       ConstantInt *LowerBound,
                                       ConstantInt *UpperBound, Value *Val,
                                       BasicBlock *Predecessor,
                                       BasicBlock *OrigBlock,
                                       BasicBlock *Default) {
  unsigned Size = End - Begin;

  if (Size == 1) {
    // Check if the Case Range is perfectly squeezed in between
    // already checked Upper and Lower bounds. If it is then we can avoid
    // emitting the code that checks if the value actually falls in the range
    // because the bounds already tell us so.
    if (Begin->Low == LowerBound && Begin->High == UpperBound) {
      fixPhis(Begin->BB, OrigBlock, Predecessor);
      return Begin->BB;
    }
    return newLeafBlock(*Begin, Val, OrigBlock, Default);
  }

  unsigned Mid = Size / 2;
  std::vector<CaseRange> LHS(Begin, Begin + Mid);
  DEBUG(dbgs() << "LHS: " << LHS << "\n");
  std::vector<CaseRange> RHS(Begin + Mid, End);
  DEBUG(dbgs() << "RHS: " << RHS << "\n");

  CaseRange &Pivot = *(Begin + Mid);
  DEBUG(dbgs() << "Pivot ==> "
               << cast<ConstantInt>(Pivot.Low)->getValue()
               << " -" << cast<ConstantInt>(Pivot.High)->getValue() << "\n");

  // NewLowerBound here should never be the integer minimal value.
  // This is because it is computed from a case range that is never
  // the smallest, so there is always a case range that has at least
  // a smaller value.
  ConstantInt *NewLowerBound = cast<ConstantInt>(Pivot.Low);
  ConstantInt *NewUpperBound;

  // If we don't have a Default block then it means that we can never
  // have a value outside of a case range, so set the UpperBound to the highest
  // value in the LHS part of the case ranges.
  if (Default != nullptr) {
    // Because NewLowerBound is never the smallest representable integer
    // it is safe here to subtract one.
    NewUpperBound = ConstantInt::get(NewLowerBound->getContext(),
                                     NewLowerBound->getValue() - 1);
  } else {
    CaseItr LastLHS = LHS.begin() + LHS.size() - 1;
    NewUpperBound = cast<ConstantInt>(LastLHS->High);
  }

  DEBUG(dbgs() << "LHS Bounds ==> ";
        if (LowerBound) {
          dbgs() << cast<ConstantInt>(LowerBound)->getSExtValue();
        } else {
          dbgs() << "NONE";
        }
        dbgs() << " - " << NewUpperBound->getSExtValue() << "\n";
        dbgs() << "RHS Bounds ==> ";
        dbgs() << NewLowerBound->getSExtValue() << " - ";
        if (UpperBound) {
          dbgs() << cast<ConstantInt>(UpperBound)->getSExtValue() << "\n";
        } else {
          dbgs() << "NONE\n";
        });

  // Create a new node that checks if the value is < pivot. Go to the
  // left branch if it is and right branch if not.
  Function* F = OrigBlock->getParent();
  BasicBlock* NewNode = BasicBlock::Create(Val->getContext(), "NodeBlock");

  ICmpInst* Comp = new ICmpInst(ICmpInst::ICMP_SLT,
                                Val, Pivot.Low, "Pivot");

  BasicBlock *LBranch = switchConvert(LHS.begin(), LHS.end(), LowerBound,
                                      NewUpperBound, Val, NewNode, OrigBlock,
                                      Default);
  BasicBlock *RBranch = switchConvert(RHS.begin(), RHS.end(), NewLowerBound,
                                      UpperBound, Val, NewNode, OrigBlock,
                                      Default);

  Function::iterator FI = OrigBlock;
  F->getBasicBlockList().insert(++FI, NewNode);
  NewNode->getInstList().push_back(Comp);

  BranchInst::Create(LBranch, RBranch, Comp, NewNode);
  return NewNode;
}

// newLeafBlock - Create a new leaf block for the binary lookup tree. It
// checks if the switch's value == the case's value. If not, then it
// jumps to the default branch. At this point in the tree, the value
// can't be another valid case value, so the jump to the "default" branch
// is warranted.
//
BasicBlock* LowerSwitch::newLeafBlock(CaseRange& Leaf, Value* Val,
                                      BasicBlock* OrigBlock,
                                      BasicBlock* Default)
{
  Function* F = OrigBlock->getParent();
  BasicBlock* NewLeaf = BasicBlock::Create(Val->getContext(), "LeafBlock");
  Function::iterator FI = OrigBlock;
  F->getBasicBlockList().insert(++FI, NewLeaf);

  // Emit comparison
  ICmpInst* Comp = nullptr;
  if (Leaf.Low == Leaf.High) {
    // Make the seteq instruction...
    Comp = new ICmpInst(*NewLeaf, ICmpInst::ICMP_EQ, Val,
                        Leaf.Low, "SwitchLeaf");
  } else {
    // Make range comparison
    if (cast<ConstantInt>(Leaf.Low)->isMinValue(true /*isSigned*/)) {
      // Val >= Min && Val <= Hi --> Val <= Hi
      Comp = new ICmpInst(*NewLeaf, ICmpInst::ICMP_SLE, Val, Leaf.High,
                          "SwitchLeaf");
    } else if (cast<ConstantInt>(Leaf.Low)->isZero()) {
      // Val >= 0 && Val <= Hi --> Val <=u Hi
      Comp = new ICmpInst(*NewLeaf, ICmpInst::ICMP_ULE, Val, Leaf.High,
                          "SwitchLeaf");      
    } else {
      // Emit V-Lo <=u Hi-Lo
      Constant* NegLo = ConstantExpr::getNeg(Leaf.Low);
      Instruction* Add = BinaryOperator::CreateAdd(Val, NegLo,
                                                   Val->getName()+".off",
                                                   NewLeaf);
      Constant *UpperBound = ConstantExpr::getAdd(NegLo, Leaf.High);
      Comp = new ICmpInst(*NewLeaf, ICmpInst::ICMP_ULE, Add, UpperBound,
                          "SwitchLeaf");
    }
  }

  // Make the conditional branch...
  BasicBlock* Succ = Leaf.BB;
  BranchInst::Create(Succ, Default, Comp, NewLeaf);

  // If there were any PHI nodes in this successor, rewrite one entry
  // from OrigBlock to come from NewLeaf.
  for (BasicBlock::iterator I = Succ->begin(); isa<PHINode>(I); ++I) {
    PHINode* PN = cast<PHINode>(I);
    // Remove all but one incoming entries from the cluster
    uint64_t Range = cast<ConstantInt>(Leaf.High)->getSExtValue() -
                     cast<ConstantInt>(Leaf.Low)->getSExtValue();    
    for (uint64_t j = 0; j < Range; ++j) {
      PN->removeIncomingValue(OrigBlock);
    }
    
    int BlockIdx = PN->getBasicBlockIndex(OrigBlock);
    assert(BlockIdx != -1 && "Switch didn't go to this successor??");
    PN->setIncomingBlock((unsigned)BlockIdx, NewLeaf);
  }

  return NewLeaf;
}

// Clusterify - Transform simple list of Cases into list of CaseRange's
unsigned LowerSwitch::Clusterify(CaseVector& Cases, SwitchInst *SI) {
  unsigned numCmps = 0;

  // Start with "simple" cases
  for (SwitchInst::CaseIt i = SI->case_begin(), e = SI->case_end(); i != e; ++i)
    Cases.push_back(CaseRange(i.getCaseValue(), i.getCaseValue(),
                              i.getCaseSuccessor()));
  
  std::sort(Cases.begin(), Cases.end(), CaseCmp());

  // Merge case into clusters
  if (Cases.size()>=2)
    for (CaseItr I = Cases.begin(), J = std::next(Cases.begin());
         J != Cases.end();) {
      int64_t nextValue = cast<ConstantInt>(J->Low)->getSExtValue();
      int64_t currentValue = cast<ConstantInt>(I->High)->getSExtValue();
      BasicBlock* nextBB = J->BB;
      BasicBlock* currentBB = I->BB;

      // If the two neighboring cases go to the same destination, merge them
      // into a single case.
      if ((nextValue-currentValue==1) && (currentBB == nextBB)) {
        I->High = J->High;
        J = Cases.erase(J);
      } else {
        I = J++;
      }
    }

  for (CaseItr I=Cases.begin(), E=Cases.end(); I!=E; ++I, ++numCmps) {
    if (I->Low != I->High)
      // A range counts double, since it requires two compares.
      ++numCmps;
  }

  return numCmps;
}

// processSwitchInst - Replace the specified switch instruction with a sequence
// of chained if-then insts in a balanced binary search.
//
void LowerSwitch::processSwitchInst(SwitchInst *SI) {
  BasicBlock *CurBlock = SI->getParent();
  BasicBlock *OrigBlock = CurBlock;
  Function *F = CurBlock->getParent();
  Value *Val = SI->getCondition();  // The value we are switching on...
  BasicBlock* Default = SI->getDefaultDest();

  // If there is only the default destination, don't bother with the code below.
  if (!SI->getNumCases()) {
    BranchInst::Create(SI->getDefaultDest(), CurBlock);
    CurBlock->getInstList().erase(SI);
    return;
  }

  const bool DefaultIsUnreachable =
      Default->size() == 1 && isa<UnreachableInst>(Default->getTerminator());
  // Create a new, empty default block so that the new hierarchy of
  // if-then statements go to this and the PHI nodes are happy.
  // if the default block is set as an unreachable we avoid creating one
  // because will never be a valid target.
  BasicBlock *NewDefault = nullptr;
  if (!DefaultIsUnreachable) {
    NewDefault = BasicBlock::Create(SI->getContext(), "NewDefault");
    F->getBasicBlockList().insert(Default, NewDefault);

    BranchInst::Create(Default, NewDefault);
  }
  // If there is an entry in any PHI nodes for the default edge, make sure
  // to update them as well.
  for (BasicBlock::iterator I = Default->begin(); isa<PHINode>(I); ++I) {
    PHINode *PN = cast<PHINode>(I);
    int BlockIdx = PN->getBasicBlockIndex(OrigBlock);
    assert(BlockIdx != -1 && "Switch didn't go to this successor??");
    PN->setIncomingBlock((unsigned)BlockIdx, NewDefault);
  }

  // Prepare cases vector.
  CaseVector Cases;
  unsigned numCmps = Clusterify(Cases, SI);

  DEBUG(dbgs() << "Clusterify finished. Total clusters: " << Cases.size()
               << ". Total compares: " << numCmps << "\n");
  DEBUG(dbgs() << "Cases: " << Cases << "\n");
  (void)numCmps;
  
  ConstantInt *UpperBound = nullptr;
  ConstantInt *LowerBound = nullptr;

  // Optimize the condition where Default is an unreachable block. In this case
  // we can make the bounds tightly fitted around the case value ranges,
  // because we know that the value passed to the switch should always be
  // exactly one of the case values.
  if (DefaultIsUnreachable) {
    CaseItr LastCase = Cases.begin() + Cases.size() - 1;
    UpperBound = cast<ConstantInt>(LastCase->High);
    LowerBound = cast<ConstantInt>(Cases.begin()->Low);
  }
  BasicBlock *SwitchBlock =
      switchConvert(Cases.begin(), Cases.end(), LowerBound, UpperBound, Val,
                    OrigBlock, OrigBlock, NewDefault);

  // Branch to our shiny new if-then stuff...
  BranchInst::Create(SwitchBlock, OrigBlock);

  // We are now done with the switch instruction, delete it.
  CurBlock->getInstList().erase(SI);

  pred_iterator PI = pred_begin(Default), E = pred_end(Default);
  // If the Default block has no more predecessors just remove it
  if (PI == E) {
    DeleteDeadBlock(Default);
  }
}
