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
   * Restituisce l'ultima istruzione appartenente al preheader, se esiste;
   * nullptr altrimenti.
   */
  Instruction *CodeMotion::getPreheaderLastInstruction() {
    if (BasicBlock *PreHeader = loop->getLoopPreheader())
      return PreHeader->getTerminator();
    return nullptr;
  }

  /**
   * Sposta l'istruzione come parametro nell'ultima posizione del preheader del
   * loop. Restituisce true se lo spostamento avviene, false altrimenti.
   */
  bool CodeMotion::moveInstrBeforeLoop(Instruction *I) {
    if (Instruction *LI = getPreheaderLastInstruction()) {
      I->moveBefore(LI);
      setOneMotionPerformed();
      return true;
    }
    return false;
  }

  /**
   * Tenta di spostare le istruzioni candidate alla code motion nel preheader
   * del loop.
   */
  void CodeMotion::executeMotion() {
    for (auto &I : *candidateInstructions) {
      graboidpasses::utils::printInstruction(
        "\033[1;38:5:196m[LICM-MOVING] Moved Instruction:"
        "\033[0m\t\033[0;38:5:196m", I, "");  // DEBUG

      if (moveInstrBeforeLoop(I))
        graboidpasses::utils::printInstruction("  -->", I);  // DEBUG
      else
        outs() << "  -->  Not moved.\n";  // DEBUG
    }
  }

} // namespace graboidpasses::licm
