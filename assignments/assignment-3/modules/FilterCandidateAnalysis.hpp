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
#include <bits/stdc++.h>

using namespace llvm;

namespace graboidpasses::licm {

  class FilterCandidateAnalysis {

  public:
    FilterCandidateAnalysis(
      Loop *L, DominatorTree *DT, const std::set<Instruction*> *II)
      : loop(L), domtree(DT), invariantInstructions(II) {}

    void filterCandidates();

    const std::set<Instruction*> &getCandidates() const {
      return candidateInstructions;
    }

  private:
    Loop *loop;
    DominatorTree *domtree;
    const std::set<Instruction*> *invariantInstructions;
    std::set<Instruction*> candidateInstructions;

    bool instructionDominatesAllExits(Instruction *I);
    void getBlocksWhereDefIsUsedOutsideLoop(
      Instruction *I, std::set<BasicBlock*> *UserBlocks);
    bool usedInsideBasicBlockRecursive(BasicBlock *BB,
      const std::set<BasicBlock*> *UserBlocks,
      std::set<BasicBlock*> *VisitedBlocks);
    bool isUsedInRegionReachableFromExitBlocks(
      const std::set<BasicBlock*> *UserBlocks);
    bool isInstructionDeadOutsideLoop(Instruction *I);
    bool isValueAssignedOnce(Instruction *I);
    bool isDefinedBeforeUse(Instruction *I);
  };

} // namespace graboidpasses::licm

#endif // GRABOID_FILTER_CANDIDATE_ANALYSIS_H
