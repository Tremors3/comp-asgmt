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

namespace graboidpasses::utils {

  void printInstruction(std::string text, Instruction *I,
    std::string terminator = "\033[0m\n");

} // namespace graboidpasses::utils

#endif // GRABOID_UTILS_H
