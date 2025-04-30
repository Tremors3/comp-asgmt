//==============================================================================
// FILE:
//    CodeMotion.cpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#include "CodeMotion.hpp"
#include "Utils.hpp"

namespace graboidpasses::licm {

  /**
   * Ottieni l'ultima istruzione del preheader.
   */
  Instruction *CodeMotion::getPreHeaderLastInstruction() {
    if (BasicBlock *PreHeader = loop->getLoopPreheader())
      return PreHeader->getTerminator();
    return nullptr;
  }

  /**
   * Effettua lo spostamento.
   */
  bool CodeMotion::moveBeforeLoop(Instruction *I) {
    if (Instruction *LI = getPreHeaderLastInstruction()) {
      I->moveBefore(LI);
      setOneMotionPerformed();
      return true;
    }
    return false;
  }

  /**
   * Muove le istruzioni candidate nel blocco precedente al loop.
   */
  void CodeMotion::executeMotion() {
    for (auto &I : *candidateInstructions) {
      graboidpasses::utils::printInstruction(
        "\033[1;38:5:196m[LICM-MOVING] Moved Instruction:"
        "\033[0m\t\033[0;38:5:196m", I, "");  // DEBUG

      if (moveBeforeLoop(I))
        graboidpasses::utils::printInstruction("  -->", I);  // DEBUG
      else
        outs() << "  -->  Not moved.\n";  // DEBUG
    }
  }

} // namespace graboidpasses::licm
