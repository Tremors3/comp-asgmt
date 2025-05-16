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

extern std::string OK;
extern std::string OK2;
extern std::string INFO;
extern std::string INFO2;
extern std::string WARNING;
extern std::string VALID;

void debug(std::string text, std::string pre = "", std::string post = "\033[0m",
           char terminator = '\n');

void debugYesNo(std::string text, bool condition,
                std::string trueCondition = "\033[0;38:5:46mYes",
                std::string falseCondition = "\033[0;38:5:196mNo", std::string pre = "",
                std::string post = "\033[0m", char terminator = '\n');

} // namespace graboidpasses::utils

#endif // GRABOID_UTILS_H
