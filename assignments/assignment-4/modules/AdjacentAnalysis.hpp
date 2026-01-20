//==============================================================================
// FILE:
//    AdjacentAnalysis.hpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_LF_ADJACENT_ANALYSIS_H
#define GRABOID_LF_ADJACENT_ANALYSIS_H
#include <llvm/Analysis/LoopInfo.h>

#include "Utils.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  class AdjacentAnalysis {
  public:
    AdjacentAnalysis(Loop *L1, Loop *L2)
        : firstLoop(L1), secondLoop(L2) {}

    bool analyse() const;

  private:
    Loop *firstLoop;
    Loop *secondLoop;

    bool processLoops() const;
    
    bool checkUnguardedLoopsAdjacency() const;

    bool checkGuardedLoopsAdjacency(
      BasicBlock *firstGuard, BasicBlock *secondGuard) const;

  /* ------------------------------------------------------------------------ */

    BasicBlock *getLoopGuard(Loop *L) const;

    bool isCleanFromUnwantedInstructions(
      BasicBlock *BB, std::set<Instruction *> whitelist) const;
    
    bool firstGuardPointsSecondGuard(
      BasicBlock *firGuard, BasicBlock *secGuard) const;
    
    Value *getGuardBranchCondition(BasicBlock *BB) const;
    
    bool areGuardsEquivalent(
      BasicBlock *firGuard, BasicBlock *secGuard) const;
  };

} // namespace graboidpasses::lf

#endif  // GRABOID_LF_ADJACENT_ANALYSIS_H
