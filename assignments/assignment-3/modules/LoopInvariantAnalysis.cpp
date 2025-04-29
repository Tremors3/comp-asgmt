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
   * Restituisce true se l'istruzione phi contiene reaching definitions
   * riferite a definizioni strettamente locate al di fuori del loop,
   * altrimenti restituisce false.
   */
  bool LoopInvariantAnalysis::phiOnlyOutDefs(PHINode *phi, Loop *L) {

    // Itero ciascuna reaching definition.
    for (unsigned j = 0; j < phi->getNumIncomingValues(); ++j) {
      Value *ReachingDef = phi->getIncomingValue(j);

      if (isa<Constant>(ReachingDef))
        return false;

      // Tutte le reaching definitions devono devono risiedere al di fuori del
      // loop; se incontriamo anche solo una reaching definition riferita ad
      // una definizione interna al loop, allora l'istruzione a cui
      // l'operando appartiene non sarà loop-invariant.
      if (Instruction *DefInst = dyn_cast<Instruction>(ReachingDef))
        if (L->contains(DefInst))
          return false;
    }

    return true;
  }

  /**
   * Restituisce true se il valore V è loop-invariant rispetto al loop L.
   * Se il valore è definito da un'istruzione all'interno del loop, verifica
   * che tale istruzione sia anch'essa loop-invariant oppure una phi con solo
   * valori di ingresso esterni (se phi_check è true).
   */
  bool LoopInvariantAnalysis::isValueLoopInvariant(
    Value *V,
    Loop *L,
    std::set<Instruction*> *invariantInstructions,
    bool phi_check)
  {
    // Le costanti sono sempre loop-invariant.
    if (isa<Constant>(V))
      return true;

    if (Instruction *I = dyn_cast<Instruction>(V)) {

      // Se l'istruzione è esterna al loop, il valore è loop-invariant.
      if (!L->contains(I))
        return true;

      // Se è una PHI, può comunque essere loop-invariant se tutti i suoi
      // input provengono da fuori dal loop (opzionale, phi_check).
      if (auto *Phi = dyn_cast<PHINode>(I)) {
        //if (phi_check && LoopInvariantAnalysis::phiOnlyOutDefs(Phi, L))
        //  return true;
        return false;
      }

      // Se l'istruzione è interna e non è loop-invariant, restituisce false.
      if (invariantInstructions->count(I) == 0)
        // CONSIGLIO PROF TODO: utilizzare una lista di visita che tiene traccia
        // delle definizioni visitate. Ogni volta che viene visitata una
        // definizione, viene aggiunta alla lista di visite. Prima di
        // controllare che la definizione sia loop-invariant dobbiamo
        // controllare che sia presente nella lista delle visitate. Se non è
        // presente, significa che non sappiamo se è loop-invariant.
        // Quindi dobbiamo iterare prima su quella. Alternativamente possiamo
        // iterare le istruzioni tramite una BFS.
        // DA IMPLEMENTARE: Controllare che una istruzione sia stata visitata
        //                  prima di controllare se è loop-invariant.
        // DA IMPLEMENTARE: Iterare le istruzioni tramite una BFS.
        return false;
    }

    return true;
  }

  /**
   * Controlla se l'istruzione corrente è loop-invariant.
   * Se lo è, restituisce true, altrimenti false.
   */
  bool LoopInvariantAnalysis::isInstructionLoopInvariant(
    Loop &L,
    Instruction *currInst,
    std::set<Instruction*> &invariantInstructionSet)
  {
    // CONSIGLIO PROF TODO: Controllo che l'istruzione sia binaria.
    if (!currInst->isBinaryOp())
      return false;

    // CONSIGLIO PROF TODO: Non castare a Use, ma castare direttamente a *Value.
    for (Use &U : currInst->operands())
      if (!LoopInvariantAnalysis::isValueLoopInvariant(
          U.get(), &L, &invariantInstructionSet))
        return false;

    return true;
  }

  /**
   * Per ciascuna istruzione del loop stabilisce se è loop-invariant.
   * In caso affermativo la aggiunge all'insieme delle istruzioni loop
   * invariant, altrimenti no.
   */
  void LoopInvariantAnalysis::markLoopInvariantInstructions(
    Loop &L,
    std::set<Instruction*> &invariantInstructionSet)
  {
    // CONSIGLIO PROF TODO: Aggiunto controllo sulla simplified form.
    if (!L.isLoopSimplifyForm()) return;

    for (auto *BB : L.getBlocks())
      for (auto &I : *BB) { // BFS
        Instruction *CurrInst = &I;

        // Se l'istruzione è loop-invariant, viene aggiunta all'insieme
        if (LoopInvariantAnalysis::isInstructionLoopInvariant(
          L, CurrInst, invariantInstructionSet))
        {
          invariantInstructionSet.insert(CurrInst);

          utils::printInstruction(
              "\033[1;38:5:033m[LICM-ANALYSIS] Invariant Instruction:\033[0m\t"
              "\033[0;38:5:033m", CurrInst
          );  // DEBUG
        }
      }

  }

} // namespace graboidpasses::licm
