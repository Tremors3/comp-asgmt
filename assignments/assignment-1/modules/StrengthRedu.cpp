//=============================================================================
// FILE:
//    StrengthRedu.cpp
//
// DESCRIPTION:
//    Strength Reduction Pass.
//
// License: GPL3
//=============================================================================
#include "llvm/IR/PassManager.h"
#include "GraboidPasses.hpp"

using namespace llvm;

namespace GraboidPasses {

  PreservedAnalyses StrengthRedu::run(Function &F, FunctionAnalysisManager &) {

    outs() << "PASSO StrengthRedu\n";

    return (false ? PreservedAnalyses::none() : PreservedAnalyses::all());
  }

} // namespace MyPasses