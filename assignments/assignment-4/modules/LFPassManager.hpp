//==============================================================================
// FILE:
//    LFPassManager.hpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_LF_PASS_MANAGER_H
#define GRABOID_LF_PASS_MANAGER_H
#include <llvm/Analysis/DependenceAnalysis.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/LoopInfo.h>

#include "NegativeDistanceDependence.hpp"
#include "ControlFlowEquivalence.hpp"
#include "TripCountEquivalence.hpp"
#include "AdjacentAnalysis.hpp"
#include "LoopFusion.hpp"
#include "Utils.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  class LFPassManager {
  public:
    LFPassManager(Function &F, FunctionAnalysisManager &AM) 
        : F(F),
          LI(AM.getResult<LoopAnalysis>(F)),
          DI(AM.getResult<DependenceAnalysis>(F)),
          DT(AM.getResult<DominatorTreeAnalysis>(F)),
          SE(AM.getResult<ScalarEvolutionAnalysis>(F)),
          PDT(AM.getResult<PostDominatorTreeAnalysis>(F)) {}

    void startPass();

    bool hasTransformed() const { return onePerformed; }

  private:
    Function &F;
    LoopInfo &LI;
    DominatorTree &DT;
    DependenceInfo &DI;
    ScalarEvolution &SE;
    PostDominatorTree &PDT;

    bool onePerformed = false;

    void iterateAndFuseLoops();
    bool analyzeCouple(Loop *FL, Loop *SL) const;
  };

} // namespace graboidpasses::lf

#endif  // GRABOID_LF_PASS_MANAGER_H
