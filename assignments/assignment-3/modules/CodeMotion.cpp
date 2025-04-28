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
  Instruction *CodeMotion::getLastInstructionBeforeLoop(Loop &L) {
      
    BasicBlock *BB = L.getHeader();
    
    // Iteratore dei predecessori del basicblock.
    auto it = pred_begin(BB);  
    
    // Aumento di uno l'iteratore per prendere 
    // il basicblock immediatamente precedente.
    BasicBlock *predecessor = *(++it);
    
    // Restituisco l'ultima istruzione del basicblock.
    return &predecessor->back();
  }
    
  /**
  * Effettua lo spostamento.
  */
  void CodeMotion::moveBeforeLoop(Instruction *I, Loop &L) {
    I->moveBefore(CodeMotion::getLastInstructionBeforeLoop(L));
  }
  
  /**
  * Muove le istruzioni candidate nel blocco precedente al loop.
  */
  void CodeMotion::moveInstructions(Loop &L, 
    std::set<Instruction*> &candidateInstructionSet) {
    for (auto &I : candidateInstructionSet){
      
      utils::printInstruction(
        "\033[1;38:5:196m[LICM-MOVING] Moved Instruction:"
        "\033[0m\t\033[0;38:5:196m", I, "");  // DEBUG
      
      CodeMotion::moveBeforeLoop(I, L);
      
      utils::printInstruction("  -->", I);  // DEBUG
    }
  }

} // namespace graboidpasses::licm