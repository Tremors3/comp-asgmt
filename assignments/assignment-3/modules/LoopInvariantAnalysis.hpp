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
#ifndef GRABOID_LICM_LOOP_INVARIANT_ANALYSIS_H
#define GRABOID_LICM_LOOP_INVARIANT_ANALYSIS_H
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/LoopInfo.h>
#include <bits/stdc++.h>

using namespace llvm;

namespace graboidpasses::licm {
  
  struct LoopInvariantAnalysis {

  private:
    bool phiOnlyOutDefs(PHINode *phi, Loop *L);
    
    bool isValueLoopInvariant(Value *V, Loop *L, 
      std::set<Instruction*> *invariantInstructions, 
      bool phi_check = false);

    bool isInstructionTypeAdmissible(Instruction *I);

    bool isInstructionLoopInvariant(Loop &L, Instruction *currInst, 
      std::set<Instruction*> &invariantInstructionSet);
  
  public:
    void markLoopInvariantInstructions(Loop &L, 
      std::set<Instruction*> &invariantInstructionSet);
    
  };
  
} // namespace graboidpasses::licm

#endif // GRABOID_LICM_LOOP_INVARIANT_ANALYSIS_H