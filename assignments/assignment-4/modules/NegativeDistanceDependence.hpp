//==============================================================================
// FILE:
//    NegativeDistanceDependence.hpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_LF_NEGATIVE_DISTANCE_DEPENDENCE_H
#define GRABOID_LF_NEGATIVE_DISTANCE_DEPENDENCE_H
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/DependenceAnalysis.h>

#include "Utils.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  class NegativeDistanceDependenceAnalysis {
  public:
    NegativeDistanceDependenceAnalysis(
      Loop *L1, Loop *L2, DependenceInfo &DI, ScalarEvolution &SE)
        : firstLoop(L1), secondLoop(L2), DI(DI), SE(SE) {}

    bool analyse() const;

  private:
    Loop *firstLoop;
    Loop *secondLoop;
    DependenceInfo &DI;
    ScalarEvolution &SE;

    void getLoopInstructionByType(
      Loop *L,
      std::vector<StoreInst *> &stores,
      std::vector<LoadInst *> &loads) const;

    bool areNegativeDistanceDependent() const;

    /* ---------------------------------------------------------------------- */

    const SCEVAddRecExpr* getSCEVAddRecExpr(Loop *L, Instruction *I) const;
    
    bool isDistanceNegativeSE(
      Loop *storeLoop, Loop *loadLoop,
      Instruction *store, Instruction *load) const;
  };

} // namespace graboidpasses::lf

#endif  // GRABOID_LF_NEGATIVE_DISTANCE_DEPENDENCE_H
