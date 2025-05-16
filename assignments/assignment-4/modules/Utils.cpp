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

std::string OK = "\033[0;38:5:40m";
std::string OK2 = "\033[1;38:5:40m";
std::string INFO = "\033[1;38:5:45m";
std::string INFO2 = "\033[1;38:5:255m";
std::string WARNING = "\033[1;38:5:226m";
std::string VALID = "\033[1;38:5:200m";

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

} // namespace graboidpasses::utils
