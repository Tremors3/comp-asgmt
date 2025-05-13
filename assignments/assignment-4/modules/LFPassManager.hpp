//==============================================================================
// FILE:
//    LFPassManager.hpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_LF_PASS_MANAGER_H
#define GRABOID_LF_PASS_MANAGER_H
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/LoopInfo.h>
#include <bits/stdc++.h>

using namespace llvm;

namespace graboidpasses::lf {

  class LFPassManager {
  public:
    void startPass();

  private:
    void processLoop(Loop &L);

  };

} // namespace graboidpasses::lf

#endif  // GRABOID_LF_PASS_MANAGER_H
