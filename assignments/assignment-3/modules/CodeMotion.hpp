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

  class CodeMotion {
  public:
    CodeMotion(Loop *L, std::set<Instruction*> *CI)
      : loop(L), candidateInstructions(CI) {};

    void executeMotion();
    bool hasOneMotionPerformed() const { return onePerformed; }

  private:
    Loop *loop;
    bool onePerformed = false;
    std::set<Instruction*> *candidateInstructions;

    bool moveBeforeLoop(Instruction *I);
    Instruction *getPreHeaderLastInstruction();

    void setOneMotionPerformed() { onePerformed = true; }

  }; // namespace graboidpasses::licm
}
#endif // GRABOID_CODE_MOTION_H
