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

void debug(std::string text, std::string pre, std::string post,
           char terminator) {
  outs() << pre << text << post << terminator; // DEBUG
}

void debugInst(std::string text, Instruction *I, std::string pre,
               std::string post, char terminator) {
  outs() << pre << text;
  I->print(outs());
  outs() << post << terminator; // DEBUG
}

void debugYesNo(std::string text, bool condition, std::string trueCondition,
                std::string falseCondition, std::string pre, std::string post,
                char terminator) {

  outs() << pre << text << (condition ? trueCondition : falseCondition) << post
         << terminator; // DEBUG
}

} // namespace graboidpasses::utils
