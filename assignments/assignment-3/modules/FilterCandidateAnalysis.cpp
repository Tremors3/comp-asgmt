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
   * Filtra le istruzioni loop-invariant per determinare se sono candidate allo
   * spostamento.
   */
  void FilterCandidateAnalysis::filterCandidates() {

    for (Instruction* I : *invariantInstructions) {

      bool definedBeforeUse = \
        FilterCandidateAnalysis::isDefinedBeforeUse(I);

      bool dominatesExits   = \
        FilterCandidateAnalysis::instructionDominatesAllExits(I);

      bool deadOutsideLoop  = \
        FilterCandidateAnalysis::isInstructionDeadOutsideLoop(I);

      bool isCandidate = \
        definedBeforeUse && (dominatesExits || deadOutsideLoop);

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
   * Controlla che l'istruzione come parametro domini tutte le uscite del loop.
   * Restituisce true se l'istruzione si trova in un blocco che domina tutte le
   * uscite del loop, altrimenti false.
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
   * Controlla che il Basic Block passato come parametro non appartenga all'
   * insieme di quelli che usano la definizione. Ricosivamente controlla anche
   * i successori del Basic Block passato come parametro. (DFS-Preorder)
   */
  bool FilterCandidateAnalysis::usedInsideBasicBlockRecursive(
    BasicBlock *BB,
    const std::set<BasicBlock*> *UserBlocks,
    std::set<BasicBlock*> *VisitedBlocks)
  {
    // Non visitiamo un nodo già visitato.
    if (VisitedBlocks->count(BB))
      return false;
    VisitedBlocks->insert(BB);

    // Copre il caso di loop annidato: seguendo
    // back-edges reincontriamo lo stesso loop.
    if (loop->contains(BB))
      return false;

    if (UserBlocks->count(BB))
      return true;

    for (BasicBlock *Succ : successors(BB))
      if (usedInsideBasicBlockRecursive(Succ, UserBlocks, VisitedBlocks))
        return true;

    return false;
  }

  /**
   * Controlla che i basic block che contengono gli utilizzatori non siano
   * raggiungibili tramite visita dei successori del loop.
   */
  bool FilterCandidateAnalysis::isUsedInRegionReachableFromExitBlocks(
    const std::set<BasicBlock*> *UserBlocks)
  {
    SmallVector<BasicBlock *, 0> ExitBlocks;
    loop->getExitBlocks(ExitBlocks);

    std::set<BasicBlock*> VisitedBlocks;

    for (BasicBlock *Exit : ExitBlocks)
      if(usedInsideBasicBlockRecursive(Exit, UserBlocks, &VisitedBlocks))
        return false;

    return true;
  }

  /**
   * Seleziona i BasicBlocks che contengono un uso dell'istruzione I al di fuori
   * del loop, e li inserisce all'interno dell'insieme UserBlocks.
   */
  void FilterCandidateAnalysis::getBlocksWhereDefIsUsedOutsideLoop(
    Instruction *I, std::set<BasicBlock*> *UserBlocks)
  {
    for (User *U : I->users())
      if (Instruction *UserInst = dyn_cast<Instruction>(U))
        if (!loop->contains(UserInst->getParent()))
          UserBlocks->insert(UserInst->getParent());
  }

  /**
   * Controlla che l'istruzione passata come parametro sia morta dopo il loop.
   * Restituisce true se l'istruzione è morta dopo il loop, altrimenti false.
   */
  bool FilterCandidateAnalysis::isInstructionDeadOutsideLoop(Instruction *I) {

    std::set<BasicBlock*> UserBlocks;

    getBlocksWhereDefIsUsedOutsideLoop(I, &UserBlocks);

    if (UserBlocks.empty())
      return true;

    return isUsedInRegionReachableFromExitBlocks(&UserBlocks);
  }

  /**
   * Controlla che la definizione rappresentata dall'istruzione come parametro
   * sia utilizzata prima dei suoi usi. Restituisce true se l'istruzione è
   * definita prima di essere usata. Se l'istruzione non domina il suo uso
   * ma l'user di quell'uso è una phi, va bene lo stesso, ritorna true.
   */
  bool FilterCandidateAnalysis::isDefinedBeforeUse(Instruction *I) {
    BasicBlock *PBB = I->getParent();

    for (User *U : I->users()) {
      Instruction *UserInst = dyn_cast<Instruction>(U);
      BasicBlock *UBB = UserInst->getParent();

      if (loop->contains(UBB)) {
        if (PBB == UBB && UserInst->comesBefore(I))
          return false;

        // Se la definizione NON DOMINA il suo uso, ma quell'uso E' operando
        // di una istruzione phi, allora va bene lo stesso, ritorna true.
        // Altrimenti se la definizione NON DOMINA il suo uso, e l'uso NON E'
        // operando di una istruzione phi, allora non va bene, ritorna false.
        if (!domtree->dominates(PBB, UBB) && !isa<PHINode>(UserInst))
          return false;
      }
    }
    return true;
  }

  /** NON UTILIZZATO. Controllo inutile data la forma SSA.
   * Controlla che il loop non contenga molteplici definizioni del valore
   * definito dall'istruzione passata come parametro.
   */
  bool FilterCandidateAnalysis::isValueAssignedOnce(Instruction *I) {

    // Se l'istruzione è utilizzata da una phi all'interno del loop, controlla
    // dove sono definite le reaching definitions della phi. Se sono tutte
    // esterne al loop, allora il valore è assegnato una sola volta. Altrimenti,
    // se anche solo una è una costante oppure una definizione interna al loop,
    // allora il valore è assegnato più di una volta.

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

} // namespace graboidpasses::licm
