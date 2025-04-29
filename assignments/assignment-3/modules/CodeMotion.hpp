//==============================================================================
// FILE:
//    CodeMotion.hpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_CODE_MOTION_H
#define GRABOID_CODE_MOTION_H
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/IR/BasicBlock.h>
#include <bits/stdc++.h>

using namespace llvm;

namespace graboidpasses::licm {

  struct CodeMotion {

  private:
    Instruction *getPreHeaderLastInstruction(Loop &L);

    bool moveBeforeLoop(Instruction *I, Loop &L);

  public:
    bool moveInstructions(Loop &L,
      std::set<Instruction*> &candidateInstructionSet);
  };

} // namespace graboidpasses::licm

#endif // GRABOID_CODE_MOTION_H
