//===-- llvm/Target/TargetMachine.h - Target Information --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the TargetMachine and LLVMTargetMachine classes.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_TARGETMACHINE_H
#define LLVM_TARGET_TARGETMACHINE_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Pass.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Target/TargetOptions.h"
#include <cassert>
#include <string>

namespace llvm {

class InstrItineraryData;
class JITCodeEmitter;
class GlobalValue;
class Mangler;
class MCAsmInfo;
class MCCodeGenInfo;
class MCContext;
class MCSymbol;
class Target;
class DataLayout;
class TargetLibraryInfo;
class TargetFrameLowering;
class TargetInstrInfo;
class TargetIntrinsicInfo;
class TargetJITInfo;
class TargetLowering;
class TargetPassConfig;
class TargetRegisterInfo;
class TargetSelectionDAGInfo;
class TargetSubtargetInfo;
class ScalarTargetTransformInfo;
class VectorTargetTransformInfo;
class formatted_raw_ostream;
class raw_ostream;

// The old pass manager infrastructure is hidden in a legacy namespace now.
namespace legacy {
class PassManagerBase;
}
using legacy::PassManagerBase;

//===----------------------------------------------------------------------===//
///
/// TargetMachine - Primary interface to the complete machine description for
/// the target machine.  All target-specific information should be accessible
/// through this interface.
///
class TargetMachine {
  TargetMachine(const TargetMachine &) LLVM_DELETED_FUNCTION;
  void operator=(const TargetMachine &) LLVM_DELETED_FUNCTION;
protected: // Can only create subclasses.
  TargetMachine(const Target &T, StringRef TargetTriple,
                StringRef CPU, StringRef FS, const TargetOptions &Options);

  /// TheTarget - The Target that this machine was created for.
  const Target &TheTarget;

  /// TargetTriple, TargetCPU, TargetFS - Triple string, CPU name, and target
  /// feature strings the TargetMachine instance is created with.
  std::string TargetTriple;
  std::string TargetCPU;
  std::string TargetFS;

  /// CodeGenInfo - Low level target information such as relocation model.
  /// Non-const to allow resetting optimization level per-function.
  MCCodeGenInfo *CodeGenInfo;

  /// AsmInfo - Contains target specific asm information.
  ///
  const MCAsmInfo *AsmInfo;

  unsigned RequireStructuredCFG : 1;

public:
  virtual ~TargetMachine();

  const Target &getTarget() const { return TheTarget; }

  const StringRef getTargetTriple() const { return TargetTriple; }
  const StringRef getTargetCPU() const { return TargetCPU; }
  const StringRef getTargetFeatureString() const { return TargetFS; }

  /// getSubtargetImpl - virtual method implemented by subclasses that returns
  /// a reference to that target's TargetSubtargetInfo-derived member variable.
  virtual const TargetSubtargetInfo *getSubtargetImpl() const {
    return nullptr;
  }

  mutable TargetOptions Options;

  /// \brief Reset the target options based on the function's attributes.
  void resetTargetOptions(const MachineFunction *MF) const;

  // Interfaces to the major aspects of target machine information:
  //
  // -- Instruction opcode and operand information
  // -- Pipelines and scheduling information
  // -- Stack frame information
  // -- Selection DAG lowering information
  //
  // N.B. These objects may change during compilation. It's not safe to cache
  // them between functions.
  virtual const TargetInstrInfo  *getInstrInfo() const { return nullptr; }
  virtual const TargetFrameLowering *getFrameLowering() const {
    return nullptr;
  }
  virtual const TargetLowering *getTargetLowering() const { return nullptr; }
  virtual const TargetSelectionDAGInfo *getSelectionDAGInfo() const {
    return nullptr;
  }
  virtual const DataLayout *getDataLayout() const { return nullptr; }

  /// getMCAsmInfo - Return target specific asm information.
  ///
  const MCAsmInfo *getMCAsmInfo() const { return AsmInfo; }

  /// getSubtarget - This method returns a pointer to the specified type of
  /// TargetSubtargetInfo.  In debug builds, it verifies that the object being
  /// returned is of the correct type.
  template<typename STC> const STC &getSubtarget() const {
    return *static_cast<const STC*>(getSubtargetImpl());
  }

  /// getRegisterInfo - If register information is available, return it.  If
  /// not, return null.  This is kept separate from RegInfo until RegInfo has
  /// details of graph coloring register allocation removed from it.
  ///
  virtual const TargetRegisterInfo *getRegisterInfo() const { return nullptr; }

  /// getIntrinsicInfo - If intrinsic information is available, return it.  If
  /// not, return null.
  ///
  virtual const TargetIntrinsicInfo *getIntrinsicInfo() const { return nullptr;}

  /// getJITInfo - If this target supports a JIT, return information for it,
  /// otherwise return null.
  ///
  virtual TargetJITInfo *getJITInfo() { return nullptr; }

  /// getInstrItineraryData - Returns instruction itinerary data for the target
  /// or specific subtarget.
  ///
  virtual const InstrItineraryData *getInstrItineraryData() const {
    return nullptr;
  }

  bool requiresStructuredCFG() const { return RequireStructuredCFG; }
  void setRequiresStructuredCFG(bool Value) { RequireStructuredCFG = Value; }

  /// getRelocationModel - Returns the code generation relocation model. The
  /// choices are static, PIC, and dynamic-no-pic, and target default.
  Reloc::Model getRelocationModel() const;

  /// getCodeModel - Returns the code model. The choices are small, kernel,
  /// medium, large, and target default.
  CodeModel::Model getCodeModel() const;

  /// getTLSModel - Returns the TLS model which should be used for the given
  /// global variable.
  TLSModel::Model getTLSModel(const GlobalValue *GV) const;

  /// getOptLevel - Returns the optimization level: None, Less,
  /// Default, or Aggressive.
  CodeGenOpt::Level getOptLevel() const;

  /// \brief Overrides the optimization level.
  void setOptLevel(CodeGenOpt::Level Level) const;

  void setFastISel(bool Enable) { Options.EnableFastISel = Enable; }

  bool shouldPrintMachineCode() const { return Options.PrintMachineCode; }

  /// getAsmVerbosityDefault - Returns the default value of asm verbosity.
  ///
  bool getAsmVerbosityDefault() const ;

  /// setAsmVerbosityDefault - Set the default value of asm verbosity. Default
  /// is false.
  void setAsmVerbosityDefault(bool);

  /// getDataSections - Return true if data objects should be emitted into their
  /// own section, corresponds to -fdata-sections.
  bool getDataSections() const;

  /// getFunctionSections - Return true if functions should be emitted into
  /// their own section, corresponding to -ffunction-sections.
  bool getFunctionSections() const;

  /// setDataSections - Set if the data are emit into separate sections.
  void setDataSections(bool);

  /// setFunctionSections - Set if the functions are emit into separate
  /// sections.
  void setFunctionSections(bool);

  /// \brief Register analysis passes for this target with a pass manager.
  virtual void addAnalysisPasses(PassManagerBase &) {}

  /// CodeGenFileType - These enums are meant to be passed into
  /// addPassesToEmitFile to indicate what type of file to emit, and returned by
  /// it to indicate what type of file could actually be made.
  enum CodeGenFileType {
    CGFT_AssemblyFile,
    CGFT_ObjectFile,
    CGFT_Null         // Do not emit any output.
  };

  /// addPassesToEmitFile - Add passes to the specified pass manager to get the
  /// specified file emitted.  Typically this will involve several steps of code
  /// generation.  This method should return true if emission of this file type
  /// is not supported, or false on success.
  virtual bool addPassesToEmitFile(PassManagerBase &,
                                   formatted_raw_ostream &,
                                   CodeGenFileType,
                                   bool /*DisableVerify*/ = true,
                                   AnalysisID /*StartAfter*/ = nullptr,
                                   AnalysisID /*StopAfter*/ = nullptr) {
    return true;
  }

  /// addPassesToEmitMachineCode - Add passes to the specified pass manager to
  /// get machine code emitted.  This uses a JITCodeEmitter object to handle
  /// actually outputting the machine code and resolving things like the address
  /// of functions.  This method returns true if machine code emission is
  /// not supported.
  ///
  virtual bool addPassesToEmitMachineCode(PassManagerBase &,
                                          JITCodeEmitter &,
                                          bool /*DisableVerify*/ = true) {
    return true;
  }

  /// addPassesToEmitMC - Add passes to the specified pass manager to get
  /// machine code emitted with the MCJIT. This method returns true if machine
  /// code is not supported. It fills the MCContext Ctx pointer which can be
  /// used to build custom MCStreamer.
  ///
  virtual bool addPassesToEmitMC(PassManagerBase &,
                                 MCContext *&,
                                 raw_ostream &,
                                 bool /*DisableVerify*/ = true) {
    return true;
  }

  void getNameWithPrefix(SmallVectorImpl<char> &Name, const GlobalValue *GV,
                         Mangler &Mang, bool MayAlwaysUsePrivate = false) const;
  MCSymbol *getSymbol(const GlobalValue *GV, Mangler &Mang) const;
};

/// LLVMTargetMachine - This class describes a target machine that is
/// implemented with the LLVM target-independent code generator.
///
class LLVMTargetMachine : public TargetMachine {
protected: // Can only create subclasses.
  LLVMTargetMachine(const Target &T, StringRef TargetTriple,
                    StringRef CPU, StringRef FS, TargetOptions Options,
                    Reloc::Model RM, CodeModel::Model CM,
                    CodeGenOpt::Level OL);

  void initAsmInfo();
public:
  /// \brief Register analysis passes for this target with a pass manager.
  ///
  /// This registers target independent analysis passes.
  void addAnalysisPasses(PassManagerBase &PM) override;

  /// createPassConfig - Create a pass configuration object to be used by
  /// addPassToEmitX methods for generating a pipeline of CodeGen passes.
  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);

  /// addPassesToEmitFile - Add passes to the specified pass manager to get the
  /// specified file emitted.  Typically this will involve several steps of code
  /// generation.
  bool addPassesToEmitFile(PassManagerBase &PM, formatted_raw_ostream &Out,
                           CodeGenFileType FileType, bool DisableVerify = true,
                           AnalysisID StartAfter = nullptr,
                           AnalysisID StopAfter = nullptr) override;

  /// addPassesToEmitMachineCode - Add passes to the specified pass manager to
  /// get machine code emitted.  This uses a JITCodeEmitter object to handle
  /// actually outputting the machine code and resolving things like the address
  /// of functions.  This method returns true if machine code emission is
  /// not supported.
  ///
  bool addPassesToEmitMachineCode(PassManagerBase &PM, JITCodeEmitter &MCE,
                                  bool DisableVerify = true) override;

  /// addPassesToEmitMC - Add passes to the specified pass manager to get
  /// machine code emitted with the MCJIT. This method returns true if machine
  /// code is not supported. It fills the MCContext Ctx pointer which can be
  /// used to build custom MCStreamer.
  ///
  bool addPassesToEmitMC(PassManagerBase &PM, MCContext *&Ctx,
                         raw_ostream &OS, bool DisableVerify = true) override;

  /// addCodeEmitter - This pass should be overridden by the target to add a
  /// code emitter, if supported.  If this is not supported, 'true' should be
  /// returned.
  virtual bool addCodeEmitter(PassManagerBase &,
                              JITCodeEmitter &) {
    return true;
  }
};

} // End llvm namespace

#endif
