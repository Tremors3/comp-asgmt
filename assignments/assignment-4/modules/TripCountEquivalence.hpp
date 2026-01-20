//==============================================================================
// FILE:
//    TripCountEquivalence.hpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_LF_TRIP_COUNT_EQUIVALENCE_H
#define GRABOID_LF_TRIP_COUNT_EQUIVALENCE_H
#include <llvm/Analysis/LoopInfo.h>

#include "Utils.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  class TripCountEquivalenceAnalysis {
  public:
    TripCountEquivalenceAnalysis(Loop *L1, Loop *L2, ScalarEvolution &SE)
        : firstLoop(L1), secondLoop(L2), SE(SE) {}

    bool analyse() const;

  private:
    Loop *firstLoop;
    Loop *secondLoop;
    ScalarEvolution &SE;

    bool areTripCountEquivalent() const;
  };

} // namespace graboidpasses::lf

#endif  // GRABOID_LF_TRIP_COUNT_EQUIVALENCE_H
