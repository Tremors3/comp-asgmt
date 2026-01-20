
//==============================================================================
// FILE:
//    ControlFlowEquivalence.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#include "ControlFlowEquivalence.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  /**
   * Entry point.
   */
  bool ControlFlowEquivalenceAnalysis::analyse() const {
    return areControlFlowEquivalent();
  }

  /**
   * Checks if the two loops are Control Flow Equivalent.
   */
  bool ControlFlowEquivalenceAnalysis::areControlFlowEquivalent() const {

    BasicBlock *firstEntry = getLoopGuard(firstLoop),
               *secondEntry = getLoopGuard(secondLoop);

    // If not all the loops are guarded, use the header instead
    if (firstEntry == nullptr || secondEntry == nullptr) {
      firstEntry = firstLoop->getHeader();
      secondEntry = secondLoop->getHeader();
    }

    return (DT.dominates(firstEntry, secondEntry) &&
            PDT.dominates(secondEntry, firstEntry));
  }

  /**
   * Returns the guard of the loop if it exists.
   */
  BasicBlock *ControlFlowEquivalenceAnalysis::getLoopGuard(Loop *L) const {
    if (BranchInst *br = L->getLoopGuardBranch())
      return br->getParent();
    return nullptr;
  }

} // namespace graboidpasses::lf
