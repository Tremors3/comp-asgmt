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

#include "Utils.hpp"

using namespace llvm;

namespace graboidpasses::licm {

  class CodeMotion {
  public:
    CodeMotion(Loop *L, const InstrMap *CI)
      : loop(L), candidateInstructionHierarchy(CI) {};

    void executeMotion();

    bool hasOneMotionPerformed() const {
      return onePerformed;
    }

  private:
    Loop *loop;
    bool onePerformed = false;
    const InstrMap *candidateInstructionHierarchy;

    Instruction *getPreheaderLastInstruction();
    bool moveInstrBeforeLoop(Instruction *I);
    void moveInstructionDFS(Instruction *I, InstrSet &Visited);

    void setOneMotionPerformed() { onePerformed = true; }

  }; // namespace graboidpasses::licm
}
#endif // GRABOID_CODE_MOTION_H
