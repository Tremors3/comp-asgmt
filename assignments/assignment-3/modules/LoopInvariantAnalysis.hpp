//==============================================================================
// FILE:
//    LoopInvariantAnalysis.hpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_LOOP_INVARIANT_ANALYSIS_H
#define GRABOID_LOOP_INVARIANT_ANALYSIS_H
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Dominators.h>
#include <bits/stdc++.h>

using namespace llvm;

namespace graboidpasses::licm {

  class LoopInvariantAnalysis {
  public:
    LoopInvariantAnalysis(Loop *L) : loop(L) {}

    void analyzeLoop();

    const std::set<Instruction*> &getInvariantInstructions() const {
      return invariantInstructions;
    }

  private:
    Loop *loop;
    std::set<Instruction*> visited;
    std::set<Instruction*> invariantInstructions;

    bool isValueLoopInvariant(Value *V);
    bool isInstrLoopInvariant(Instruction *I);
  };

} // namespace graboidpasses::licm

#endif // GRABOID_LOOP_INVARIANT_ANALYSIS_H
