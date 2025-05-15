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
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Analysis/LoopInfo.h>

using namespace llvm;

namespace graboidpasses::utils {

void debug(std::string text, std::string pre = "", std::string post = "",
           char terminator = '\n');

void debugYesNo(std::string text, bool condition,
                std::string trueCondition = "Yes",
                std::string falseCondition = "No", std::string pre = "",
                std::string post = "\033[0m", char terminator = '\n');

} // namespace graboidpasses::utils

#endif // GRABOID_UTILS_H
