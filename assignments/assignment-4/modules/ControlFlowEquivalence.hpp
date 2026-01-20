//==============================================================================
// FILE:
//    ControlFlowEquivalence.hpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_LF_CONTROL_FLOW_EQUIVALENCE_H
#define GRABOID_LF_CONTROL_FLOW_EQUIVALENCE_H
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/PostDominators.h>

#include "Utils.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  class ControlFlowEquivalenceAnalysis {
  public:
    ControlFlowEquivalenceAnalysis(
      Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT)
        : firstLoop(L1), secondLoop(L2), DT(DT), PDT(PDT) {}

    bool analyse() const;

  private:
    Loop *firstLoop;
    Loop *secondLoop;
    DominatorTree &DT;
    PostDominatorTree &PDT;

    bool areControlFlowEquivalent() const;
    BasicBlock *getLoopGuard(Loop *L) const;
  };

} // namespace graboidpasses::lf

#endif  // GRABOID_LF_CONTROL_FLOW_EQUIVALENCE_H
