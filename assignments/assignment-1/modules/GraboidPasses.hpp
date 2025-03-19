//=============================================================================
// FILE:
//    GraboidPasses.hpp
//
// DESCRIPTION:
//    Header file that prototypes the three passes.
//
// License: GPL3
//=============================================================================
#pragma once
#ifndef MY_PASSES_H
#define MY_PASSES_H

#include "llvm/IR/PassManager.h"

using namespace llvm;

namespace GraboidPasses {
    
  struct AlgebricIden: PassInfoMixin<AlgebricIden> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    static bool isRequired() { return true; }
  };

  struct MultiInstOpt: PassInfoMixin<MultiInstOpt> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    static bool isRequired() { return true; }
  };

  struct StrengthRedu: PassInfoMixin<StrengthRedu> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    static bool isRequired() { return true; }
  };

} // namespace

#endif // MY_PASSES_H