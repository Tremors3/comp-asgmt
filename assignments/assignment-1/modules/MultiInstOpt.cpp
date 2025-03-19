//=============================================================================
// FILE:
//    MultiInstOpt.cpp
//
// DESCRIPTION:
//    Multi-Instruction Optimization Pass.
//
// License: GPL3
//=============================================================================
#include "llvm/IR/PassManager.h"
#include "GraboidPasses.hpp"

using namespace llvm;

namespace GraboidPasses {

  PreservedAnalyses MultiInstOpt::run(Function &F, FunctionAnalysisManager &) {

    outs() << "PASSO MultiInstOpt\n";

    return (false ? PreservedAnalyses::none() : PreservedAnalyses::all());
  }

} // namespace MyPasses