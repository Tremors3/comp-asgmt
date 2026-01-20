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
#include <llvm/IR/Instructions.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include "llvm/Analysis/ScalarEvolutionExpressions.h"

using namespace llvm;

namespace graboidpasses::utils {

extern std::string DARK_GREEN;
extern std::string GREEN;
extern std::string BLUE;
extern std::string WHITE;
extern std::string YELLOW;
extern std::string PURPLE;
extern std::string RESET;

extern char NO_TERM;

void debug(std::string text, std::string pre = "", std::string post = "\033[0m",
           char terminator = '\n');

void debugYesNo(std::string text, bool condition,
                std::string trueCondition = "\033[0;38:5:46mYes",
                std::string falseCondition = "\033[0;38:5:196mNo",
                std::string pre = "",
                std::string post = "\033[0m",
                char terminator = '\n');

void debugValue(std::string text,
                Value *V,
                std::string pre = "",
                std::string post = "\033[0m",
                char terminator = '\n');

void debugSCEV(std::string text,
                const SCEV *S,
                std::string pre = "",
                std::string post = "\033[0m",
                char terminator = '\n');

} // namespace graboidpasses::utils

#endif // GRABOID_UTILS_H
