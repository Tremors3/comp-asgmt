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

namespace graboidpasses::licm {

  /**
   * Itera le istruzioni appartenenti al loop e decreta se sono loop-invariant
   * o meno. In caso affermativo le aggiunge ad un set, per marcarle.
   */
  void LoopInvariantAnalysis::analyzeLoop() {
    if (!loop->isLoopSimplifyForm())
      return;

    InstrSet Visited;

    // Pulisce la gerarchia precedente
    invariantInstructionHierarchy.clear();

    // Itera le istruzioni di ogni BB appartenente al loop
    for (BasicBlock *BB : loop->getBlocks())
      for (Instruction &I : *BB)
        isInstructionLoopInvariant(&I, Visited);
  }

  /**
   * Controlla che l'istruzione come parametro sia loop-invariant.
   * In caso affermativo restituisce true, altrimenti false.
   */
  bool LoopInvariantAnalysis::isInstructionLoopInvariant(
    Instruction *I, InstrSet &Visited) {

    // Se già visitata, ritorna se era considerata invariant
    if (Visited.count(I))
      return invariantInstructionHierarchy.count(I);

    Visited.insert(I);

    // Solo le operazioni binarie sono considerate per questa analisi
    if (!I->isBinaryOp())
      return false;

    // Insieme temporaneo delle dipendenze loop-invariant dell'istruzione
    InstrSet deps;

    // Controlla tutti gli operandi
    for (Value *Op : I->operands()) {
      if (!isValueLoopInvariant(Op, Visited))
        return false;

      // Se l'operando è un'istruzione nel loop ed
      // è loop-invariant, allora è una dipendenza
      if (Instruction *OpI = dyn_cast<Instruction>(Op))
        if (loop->contains(OpI))
          deps.insert(OpI);
    }

    // L'istruzione è loop-invariant, quindi aggiorniamo la gerarchia
    invariantInstructionHierarchy[I] = std::move(deps);

    printInstruction(
      "\033[1;38:5:033m[LICM-ANALYSIS] Invariant Instruction:\033[0m\t"
      "\033[0;38:5:033m", I
    ); // DEBUG

    return true;
  }

  /**
   * Controlla che il valore come parametro sia loop-invariant.
   * In caso affermativo restituisce true, altrimenti false.
   */
  bool LoopInvariantAnalysis::isValueLoopInvariant(
    Value *V, InstrSet &Visited) {

    // Le costanti sono invariant
    if (isa<Constant>(V))
      return true;

    // Assumiamo che se non si tratta di un'istruzione,
    // allora non è invariant
    Instruction *I = dyn_cast<Instruction>(V);
    if (!I) return false;

    // Istruzione fuori dal loop quindi invariant
    if (!loop->contains(I))
      return true;

    // PHI nodes interni al loop non sono invariant
    if (isa<PHINode>(I))
      return false;

    // Controlliamo gli operandi ricorsivamente
    return isInstructionLoopInvariant(I, Visited);
  }

} // namespace graboidpasses::licm
