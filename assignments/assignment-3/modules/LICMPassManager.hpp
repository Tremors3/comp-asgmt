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

  class LicmPassManager {
  public:
    LicmPassManager(LoopInfo* LI, DominatorTree *DT)
      : loopinfo(LI), domtree(DT) {}

    void startPass();
    bool hasTransformed() const { return onePerformed; }

  private:
    LoopInfo *loopinfo;
    DominatorTree *domtree;
    bool onePerformed = false;

    void processLoop(Loop &L);
    void iterateTopLevelLoops();
    void iterateSubLevelLoops(Loop &L);

    void setTransformed() { onePerformed = true; }
  };

} // namespace graboidpasses::licm

#endif  // GRABOID_LICM_PASS_MANAGER_H
