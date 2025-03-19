//=============================================================================
// FILE:
//    AlgebricIden.cpp
//
// DESCRIPTION:
//    Algebric Identity Pass.
//
// License: GPL3
//=============================================================================
#include "llvm/IR/PassManager.h"
#include "GraboidPasses.hpp"

using namespace llvm;

namespace GraboidPasses {

  PreservedAnalyses AlgebricIden::run(Function &F, FunctionAnalysisManager &) {

    outs() << "PASSO AlgebricIden\n";

    return (false ? PreservedAnalyses::none() : PreservedAnalyses::all());
  }

} // namespace MyPasses