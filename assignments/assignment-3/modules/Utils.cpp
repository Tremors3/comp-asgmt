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

namespace graboidpasses::utils{

  void printInstruction(
    std::string text, Instruction *I, std::string terminator) {
    outs() << text; I->print(outs()); outs() << terminator;  // DEBUG
  }

} // namespace graboidpasses::utils