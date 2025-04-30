//==============================================================================
// FILE:
//    FilterCandidateAnalysis.hpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_FILTER_CANDIDATE_ANALYSIS_H
#define GRABOID_FILTER_CANDIDATE_ANALYSIS_H
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/Analysis/LoopInfo.h>
#include <bits/stdc++.h>

using namespace llvm;

namespace graboidpasses::licm {

  struct FilterCandidateAnalysis {
  private:
    bool instructionDominatesAllExits(Instruction *I, Loop &L,
        DominatorTree &DT);

    bool isVariableDeadOutsideLoop(Instruction *I, Loop &L);

    bool isValueAssignedOnce(Instruction *I, Loop &L);

    bool isDefinedBeforeUse(Instruction *I, Loop &L, DominatorTree &DT);

  public:
    void filterInvariantInstructions(
      Loop &L, DominatorTree &DT,
      std::set<Instruction*> &invariantInstructionSet,
      std::set<Instruction*> &candidateInstructionSet
    );

  };

} // namespace graboidpasses::licm

#endif // GRABOID_FILTER_CANDIDATE_ANALYSIS_H
