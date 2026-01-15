//==============================================================================
// FILE:
//    Utils.hpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_UTILS_H
#define GRABOID_UTILS_H
#include <bits/stdc++.h>
#include <llvm/IR/Instructions.h>

using namespace llvm;

namespace graboidpasses::licm {
  
  // Alias per un insieme di istruzioni
  using InstrSet = std::unordered_set<Instruction*>;

  // Alias per una mappa da istruzione a insieme di istruzioni
  using InstrMap = std::unordered_map<Instruction*, InstrSet>;
  
  void printInstruction(std::string text, Instruction *I,
    std::string terminator = "\033[0m\n");
  
} // namespace graboidpasses::licm

#endif // GRABOID_UTILS_H
