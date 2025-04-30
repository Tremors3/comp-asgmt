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
#include <llvm/IR/Instructions.h>
#include <llvm/Analysis/LoopInfo.h>
#include <bits/stdc++.h>

using namespace llvm;

namespace graboidpasses::licm {

  class FilterCandidateAnalysis {

  public:
    FilterCandidateAnalysis(Loop *L, DominatorTree *DT, std::set<Instruction*> *II)
      : loop(L), domtree(DT), invariantInstructions(II) {}

    void filterCandidates();

    const std::set<Instruction*> &getCandidates() const {
      return candidateInstructions;
    }

  private:
    Loop *loop;
    DominatorTree *domtree;
    std::set<Instruction*> *invariantInstructions;
    std::set<Instruction*> candidateInstructions;

    bool instructionDominatesAllExits(Instruction *I);
    bool isVariableDeadOutsideLoop(Instruction *I);
    bool isValueAssignedOnce(Instruction *I);
    bool isDefinedBeforeUse(Instruction *I);
  };

} // namespace graboidpasses::licm

#endif // GRABOID_FILTER_CANDIDATE_ANALYSIS_H
