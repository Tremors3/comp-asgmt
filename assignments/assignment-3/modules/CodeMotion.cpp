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

namespace graboidpasses::licm {

  /**
   * Itera su tutte le istruzioni candidate alla code motion e le sposta
   * nel preheader del loop rispettando l'ordine delle dipendenze.
   *
   * Viene applicata una DFS post-order sulla gerarchia, in modo che ogni
   * istruzione venga spostata solo dopo le sue dipendenze.
   */
  void CodeMotion::executeMotion() {
    InstrSet Visited;

    for (const auto &[I, _] : *candidateInstructionHierarchy)
      moveInstructionDFS(I, Visited);
  }

  /**
   * Esegue la DFS post-order su una singola istruzione candidata.
   * Vengono prima spostate tutte le dipendenze, poi l'istruzione stessa.
   */
  void CodeMotion::moveInstructionDFS(
    Instruction *I, InstrSet &Visited) {

    if (Visited.count(I))
      return;

    Visited.insert(I);

    // 1. Recupera le dipendenze della gerarchia candidate
    auto It = candidateInstructionHierarchy->find(I);
    if (It != candidateInstructionHierarchy->end()) {
      for (Instruction *Dep : It->second)
        moveInstructionDFS(Dep, Visited); // sposta prima le dipendenze
    }

    // 2. Sposta l'istruzione nel preheader
    printInstruction(
        "\033[1;38:5:196m[LICM-MOVING] Moved Instruction:"
        "\033[0m\t\033[0;38:5:196m", I, "");  // DEBUG
    
    if (moveInstrBeforeLoop(I)) {
      setOneMotionPerformed();
      printInstruction("  -->", I);  // DEBUG
    } else {
      outs() << "  -->  Not moved.\n";  // DEBUG
    }
  }

  /**
   * Sposta l'istruzione nell'ultima posizione del preheader del loop.
   * Ritorna true se lo spostamento è avvenuto, false altrimenti.
   */
  bool CodeMotion::moveInstrBeforeLoop(Instruction *I) {
    if (Instruction *LI = getPreheaderLastInstruction()) {
      I->moveBefore(LI);
      return true;
    }
    return false;
  }

  /**
   * Restituisce l'ultima istruzione del preheader del loop,
   * se esiste. Altrimenti ritorna nullptr.
   */
  Instruction *CodeMotion::getPreheaderLastInstruction() {
    if (BasicBlock *PreHeader = loop->getLoopPreheader())
      return PreHeader->getTerminator();
    return nullptr;
  }

} // namespace graboidpasses::licm
