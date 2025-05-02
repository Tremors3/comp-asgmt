//==============================================================================
// FILE:
//    LoopInvariantAnalysis.cpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#include "LoopInvariantAnalysis.hpp"
#include "Utils.hpp"

namespace graboidpasses::licm {

  /**
   * Itera le istruzioni appartenenti al loop e decreta se sono loop-invariant
   * o meno. In caso affermativo le aggiunge ad un set, per marcarle.
   */
  void LoopInvariantAnalysis::analyzeLoop() {
    if (!loop->isLoopSimplifyForm())
      return;

    for (BasicBlock *BB : loop->getBlocks()) {
      for (Instruction &I : *BB) {
        Instruction *Inst = &I;

        if ( (!visited.count(Inst)) && isInstrLoopInvariant(Inst) ) {
          invariantInstructions.insert(Inst);

          utils::printInstruction(
            "\033[1;38:5:033m[LICM-ANALYSIS] Invariant Instruction:\033[0m\t"
            "\033[0;38:5:033m", Inst
          ); // DEBUG
        }
      }
    }
  }

  /**
   * Controlla che l'istruzione come parametro sia loop-invariant.
   * In caso affermativo restituisce true, altrimenti false.
   */
  bool LoopInvariantAnalysis::isInstrLoopInvariant(Instruction *I) {
    if (visited.count(I))
      return invariantInstructions.count(I);

    visited.insert(I);

    if (!I->isBinaryOp())
      return false;

    for (Value *Operand : I->operands()) {
      if (!isValueLoopInvariant(Operand))
        return false;
    }

    return true;
  }

  /**
   * Controlla che il valore come parametro sia loop-invariant.
   * In caso affermativo restituisce true, altrimenti false.
   */
  bool LoopInvariantAnalysis::isValueLoopInvariant(Value *V) {
    if (isa<Constant>(V))
      return true;

    if (Instruction *I = dyn_cast<Instruction>(V)) {
      if (!loop->contains(I))
        return true;

      if (isa<PHINode>(I))
        return false;

      if (!visited.count(I))
        // L'istruzione che definisce l'operando non e' stata ancora visitata.
        // La visitiamo per decretare che sia loop-invariant, o meno.
        return isInstrLoopInvariant(I);

      // L'istruzione che definisce l'operando è già stata visitata.
      return invariantInstructions.count(I);
    }

    return false;
  }

} // namespace graboidpasses::licm
