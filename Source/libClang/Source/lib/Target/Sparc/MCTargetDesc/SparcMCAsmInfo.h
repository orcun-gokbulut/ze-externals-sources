//===-- SparcMCAsmInfo.h - Sparc asm properties ----------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the SparcMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef SPARCTARGETASMINFO_H
#define SPARCTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class StringRef;

class SparcELFMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;
public:
  explicit SparcELFMCAsmInfo(StringRef TT);
  const MCExpr*
  getExprForPersonalitySymbol(const MCSymbol *Sym, unsigned Encoding,
                              MCStreamer &Streamer) const override;
  const MCExpr* getExprForFDESymbol(const MCSymbol *Sym,
                                    unsigned Encoding,
                                    MCStreamer &Streamer) const override;

};

} // namespace llvm

#endif
