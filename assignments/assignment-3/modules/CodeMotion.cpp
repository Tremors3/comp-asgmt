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
  Instruction *CodeMotion::getPreHeaderLastInstruction(Loop &L) {
    if (BasicBlock *PreHeader = L.getLoopPreheader()) {
      return PreHeader->getTerminator();
    }
    return nullptr;
  }

  /**
   * Effettua lo spostamento.
   */
  bool CodeMotion::moveBeforeLoop(Instruction *I, Loop &L) {
    if (Instruction *LI = CodeMotion::getPreHeaderLastInstruction(L)) {
      I->moveBefore(LI);
      return true;
    }
    return false;
  }

  /**
   * Muove le istruzioni candidate nel blocco precedente al loop.
   */
  bool CodeMotion::moveInstructions(
    Loop &L,
    std::set<Instruction*> &candidateInstructionSet)
  {
    bool Transformed = false;

    for (auto &I : candidateInstructionSet){

      utils::printInstruction(
        "\033[1;38:5:196m[LICM-MOVING] Moved Instruction:"
        "\033[0m\t\033[0;38:5:196m", I, "");  // DEBUG

      bool CurrTransformed = CodeMotion::moveBeforeLoop(I, L);

      Transformed |= CurrTransformed;

      if (CurrTransformed)
        utils::printInstruction("  -->", I);  // DEBUG
      else
        outs() << "  -->  Not moved.\n";  // DEBUG
    }

    return Transformed;
  }

} // namespace graboidpasses::licm
