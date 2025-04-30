//==============================================================================
// FILE:
//    FilterCandidateAnalysis.cpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#include "FilterCandidateAnalysis.hpp"
#include "Utils.hpp"

namespace graboidpasses::licm {

  /**
   * Filtra le istruzioni loop-invariant per determinare se sono
   * candidate a essere spostate all'esterno del loop.
   */
  void FilterCandidateAnalysis::filterCandidates() {

    for (Instruction* I : *invariantInstructions) {

      bool assignedOnce     = \
        FilterCandidateAnalysis::isValueAssignedOnce(I);

      bool definedBeforeUse = \
        FilterCandidateAnalysis::isDefinedBeforeUse(I);

      bool dominatesExits   = \
        FilterCandidateAnalysis::instructionDominatesAllExits(I);

      bool deadOutsideLoop  = \
        FilterCandidateAnalysis::isVariableDeadOutsideLoop(I);

      bool isCandidate = assignedOnce && definedBeforeUse &&
                        (dominatesExits || deadOutsideLoop);

      if (isCandidate) {
        candidateInstructions.insert(I);

        utils::printInstruction(
          "\033[1;38:5:214m[LICM-FILTERING] Movable Instruction:\033[0m\t"
          "\033[0;38:5:214m", I
        );  // DEBUG
      }
    }
  }

  /**
   * Restituisce true se l'istruzione I si trova in un blocco che
   * domina tutte le uscite del loop L, altrimenti false.
   */
  bool FilterCandidateAnalysis::instructionDominatesAllExits(Instruction *I)
  {
    SmallVector<BasicBlock *, 0> ExitBlocks;
    loop->getExitBlocks(ExitBlocks);

    for (BasicBlock *Exit : ExitBlocks)
      if (!domtree->dominates(I->getParent(), Exit))
        return false;

    return true;
  }

  /**
   * Restituisce true se l'istruzione I è morta al di fuori del loop L. Se
   * tutti gli user di I sono all'interno del loop, allora I è morta al di
   * fuori del loop.
   */
  bool FilterCandidateAnalysis::isVariableDeadOutsideLoop(Instruction *I) {
    // SmallVector<BasicBlock *, 0> ExitBlocks;
    // L.getExitBlocks(ExitBlocks);
    // for (BasicBlock *Exit : ExitBlocks)
    //   ;

    for (auto *U : I->users())
      if (Instruction *UserInst = dyn_cast<Instruction>(U))

        // CONSIGLIO PROF TODO:
        // - SCELTA MIGLIORE: Il prof consiglia di utilizzare i successori della
        // classe basic block per controllare che l'utilizzo dell'istruzione sia
        // utilizzata al di fuori del loop. (successori delle uscite del loop).
        // - ALTERNATIVA CHE NON FUNZIONA: Il prof sconsiglia di risolvere il
        // problema controllando che il blocco che contiene l'uso sia dominato
        // dalle uscite del loop, perchè da dei problemi quando un'uscita è
        // condivisa da più branch entranti.
        // Le definizioni dominano sempre i propri usi. Allora perchè non va bene?
        // - ALTERNATIVA CORRENTE (NON ESATTA): Il problema consiste nel fatto
        // che l'uso potrebbe essere definito anche prima del loop; per questo
        // motivo ci sarebbe bisogno di controllare che il basic block che
        // contiene l'uso non sia solo esterno al loop, ma che venga anche dopo
        // di esso, non prima. Ma questo ci riporta al problema risolvibile
        // tramite la soluzione del prof.

        if (!loop->contains(UserInst->getParent()))
          return false;
    return true;
  }

  /**
   * Se l'istruzione I è utilizzata da una phi all'interno del loop,
   * controlla dove sono definite le reaching definitions della phi.
   * Se sono tutte esterne al loop, allora I è assegnato una sola volta.
   * Altrimenti, se anche solo una è una costante oppure una definizione
   * interna al loop, allora I è assegnato più di una volta.
   */
  bool FilterCandidateAnalysis::isValueAssignedOnce(Instruction *I) {

    // CONSIGLIO PROF TODO: Il prof consiglia di rimuovere questa funzione
    // perchè inutile. Dobbiamo ragionare sulla possibilità che due definizioni
    // collidano in una phi, e che spostarle entrambe nel preheader può essere
    // dannoso.

    for (User *User : I->users()) {

      if (isa<PHINode>(User) && loop->contains(dyn_cast<Instruction>(User))) {
        PHINode *phi = dyn_cast<PHINode>(User);

        for (unsigned j = 0; j < phi->getNumIncomingValues(); ++j) {
          Value *ReachingDef = phi->getIncomingValue(j);

          if (isa<Constant>(ReachingDef))
            return false;

          if (Instruction *ReachingDefInst = dyn_cast<Instruction>(ReachingDef))
            if (ReachingDefInst != I && loop->contains(ReachingDefInst))
              return false;
        }
      }
    }

    return true;
  }

  /**
   * Restituisce true se l'istruzione I è definita prima di essere usata.
   */
  bool FilterCandidateAnalysis::isDefinedBeforeUse(Instruction *I) {
    BasicBlock *PBB = I->getParent();

    for (User *U : I->users()) {
      Instruction *UserInst = dyn_cast<Instruction>(U);
      BasicBlock *UBB = UserInst->getParent();

      if (loop->contains(UBB)) {

        if (PBB == UBB && UserInst->comesBefore(I))
          return false;

        // Se il Basic Block padre della definizione domina quello dell'
        // utilizzatore e l'utilizzatore non è un'istruzione di tipo phi,
        // allora restituisce falso; altrimenti restituisce vero.
        if (!domtree->dominates(PBB, UBB) && !isa<PHINode>(UserInst))
          return false;

      }

    }

    return true;
  }

} // namespace graboidpasses::licm
