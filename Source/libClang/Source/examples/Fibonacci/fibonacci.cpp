//===--- examples/Fibonacci/fibonacci.cpp - An example use of the JIT -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This small program provides an example of how to build quickly a small module
// with function Fibonacci and execute it with the JIT.
//
// The goal of this snippet is to create in the memory the LLVM module
// consisting of one function as follow:
//
//   int fib(int x) {
//     if(x<=2) return 1;
//     return fib(x-1)+fib(x-2);
//   }
//
// Once we have this, we compile the module via JIT, then execute the `fib'
// function and return result to a driver, i.e. to a "host program".
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

static Function *CreateFibFunction(Module *M, LLVMContext &Context) {
  // Create the fib function and insert it into module M. This function is said
  // to return an int and take an int parameter.
  Function *FibF =
    cast<Function>(M->getOrInsertFunction("fib", Type::getInt32Ty(Context),
                                          Type::getInt32Ty(Context),
                                          (Type *)0));

  // Add a basic block to the function.
  BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", FibF);

  // Get pointers to the constants.
  Value *One = ConstantInt::get(Type::getInt32Ty(Context), 1);
  Value *Two = ConstantInt::get(Type::getInt32Ty(Context), 2);

  // Get pointer to the integer argument of the add1 function...
  Argument *ArgX = FibF->arg_begin();   // Get the arg.
  ArgX->setName("AnArg");            // Give it a nice symbolic name for fun.

  // Create the true_block.
  BasicBlock *RetBB = BasicBlock::Create(Context, "return", FibF);
  // Create an exit block.
  BasicBlock* RecurseBB = BasicBlock::Create(Context, "recurse", FibF);

  // Create the "if (arg <= 2) goto exitbb"
  Value *CondInst = new ICmpInst(*BB, ICmpInst::ICMP_SLE, ArgX, Two, "cond");
  BranchInst::Create(RetBB, RecurseBB, CondInst, BB);

  // Create: ret int 1
  ReturnInst::Create(Context, One, RetBB);

  // create fib(x-1)
  Value *Sub = BinaryOperator::CreateSub(ArgX, One, "arg", RecurseBB);
  CallInst *CallFibX1 = CallInst::Create(FibF, Sub, "fibx1", RecurseBB);
  CallFibX1->setTailCall();

  // create fib(x-2)
  Sub = BinaryOperator::CreateSub(ArgX, Two, "arg", RecurseBB);
  CallInst *CallFibX2 = CallInst::Create(FibF, Sub, "fibx2", RecurseBB);
  CallFibX2->setTailCall();


  // fib(x-1)+fib(x-2)
  Value *Sum = BinaryOperator::CreateAdd(CallFibX1, CallFibX2,
                                         "addresult", RecurseBB);

  // Create the return instruction and add it to the basic block
  ReturnInst::Create(Context, Sum, RecurseBB);

  return FibF;
}


int main(int argc, char **argv) {
  int n = argc > 1 ? atol(argv[1]) : 24;

  InitializeNativeTarget();
  LLVMContext Context;

  // Create some module to put our function into it.
  std::unique_ptr<Module> M(new Module("test", Context));

  // We are about to create the "fib" function:
  Function *FibF = CreateFibFunction(M.get(), Context);

  // Now we going to create JIT
  std::string errStr;
  ExecutionEngine *EE =
    EngineBuilder(M.get())
    .setErrorStr(&errStr)
    .setEngineKind(EngineKind::JIT)
    .create();

  if (!EE) {
    errs() << argv[0] << ": Failed to construct ExecutionEngine: " << errStr
           << "\n";
    return 1;
  }

  errs() << "verifying... ";
  if (verifyModule(*M)) {
    errs() << argv[0] << ": Error constructing function!\n";
    return 1;
  }

  errs() << "OK\n";
  errs() << "We just constructed this LLVM module:\n\n---------\n" << *M;
  errs() << "---------\nstarting fibonacci(" << n << ") with JIT...\n";

  // Call the Fibonacci function with argument n:
  std::vector<GenericValue> Args(1);
  Args[0].IntVal = APInt(32, n);
  GenericValue GV = EE->runFunction(FibF, Args);

  // import result of execution
  outs() << "Result: " << GV.IntVal << "\n";

  return 0;
}
