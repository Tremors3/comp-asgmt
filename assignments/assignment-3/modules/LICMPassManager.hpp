//==============================================================================
// FILE:
//    LICMPassManager.hpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_LICM_PASS_MANAGER_H
#define GRABOID_LICM_PASS_MANAGER_H
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/LoopInfo.h>
#include <bits/stdc++.h>

using namespace llvm;

namespace graboidpasses::licm {

  struct LicmPassManager: PassInfoMixin<LicmPassManager> {

  private:
    bool processLoop(Loop &L, DominatorTree &DT);

    bool iterateSubLevelLoops(Loop &L, DominatorTree &DT);

    bool iterateTopLevelLoops(LoopInfo &LI, DominatorTree &DT);

/*----------------------------------------------------------------------------*/

  private:
    bool runOnFunction(Function &F, FunctionAnalysisManager &AM);

  public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
  };

} // namespace graboidpasses::licm

#endif  // GRABOID_LICM_PASS_MANAGER_H
