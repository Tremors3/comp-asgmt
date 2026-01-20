
//==============================================================================
// FILE:
//    TripCountEquivalence.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#include "TripCountEquivalence.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  /**
   * Entry point.
   */
  bool TripCountEquivalenceAnalysis::analyse() const {
    return areTripCountEquivalent();
  }

    /**
   * Checks if the two loops iterate the same number of times.
   * @see <a
   * href="https://llvm.org/devmtg/2018-04/slides/Absar-ScalarEvolution.pdf">
   * Scalar Evolution </a>
   */
  bool TripCountEquivalenceAnalysis::areTripCountEquivalent() const {

    if (!SE.hasLoopInvariantBackedgeTakenCount(firstLoop) ||
        !SE.hasLoopInvariantBackedgeTakenCount(secondLoop)) {
      utils::debug("[LF-TCE] At least one of the loops hasn't got a loop "
                   "invariant backedge.",
                   utils::YELLOW);
      return false;
    }

    const SCEV *l1scev = SE.getBackedgeTakenCount(firstLoop),
               *l2scev = SE.getBackedgeTakenCount(secondLoop);

    if (isa<SCEVCouldNotCompute>(l1scev) || isa<SCEVCouldNotCompute>(l2scev)) {
      utils::debug("[LF-TCE] The backedge is not of the correct type.",
                   utils::YELLOW);
      return false;
    }

    if (l1scev != l2scev) {

      unsigned t1 = SE.getSmallConstantTripMultiple(firstLoop, l1scev),
               t2 = SE.getSmallConstantTripMultiple(secondLoop, l2scev);

      utils::debug("[LF-TCE] The trip count of the two loops is different: (" +
                       std::to_string(t1) + " != " + std::to_string(t2) + ").",
                   utils::YELLOW);

      return false;
    }

    return true;
  }

} // namespace graboidpasses::lf
