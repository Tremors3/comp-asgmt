//==============================================================================
// FILE:
//    Utils.cpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#include "Utils.hpp"

namespace graboidpasses::utils {

std::string DARK_GREEN = "\033[0;38:5:40m";
std::string GREEN = "\033[1;38:5:40m";
std::string BLUE = "\033[1;38:5:45m";
std::string WHITE = "\033[1;38:5:255m";
std::string YELLOW = "\033[1;38:5:226m";
std::string PURPLE = "\033[1;38:5:200m";
std::string RESET = "\033[0m";

char NO_TERM = '\0';

void debug(std::string text, std::string pre, std::string post,
           char terminator) {
  outs() << pre << text << post << terminator; // DEBUG
}

void debugYesNo(std::string text, bool condition, std::string trueCondition,
                std::string falseCondition, std::string pre, std::string post,
                char terminator) {

  outs() << pre << text << (condition ? trueCondition : falseCondition) << post
         << terminator; // DEBUG
}

void debugValue(
  std::string text, Value *V, std::string pre, std::string post, char terminator) {
  outs() << pre << text; V->print(outs(), true); outs() << post << terminator;  // DEBUG
}

void debugSCEV(
  std::string text, const SCEV *S, std::string pre, std::string post, char terminator) {
  outs() << pre << text; S->print(outs()); outs() << post << terminator;  // DEBUG
}

} // namespace graboidpasses::utils
