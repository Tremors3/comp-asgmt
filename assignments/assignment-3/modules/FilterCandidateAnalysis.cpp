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
   * Restituisce true se l'istruzione I si trova in un blocco che
   * domina tutte le uscite del loop L, altrimenti false.
   */
  bool FilterCandidateAnalysis::instructionDominatesAllExits(
    Instruction *I, Loop &L, DominatorTree &DT) 
  {
    
    SmallVector<BasicBlock *, 0> ExitBlocks;
    L.getExitBlocks(ExitBlocks);

    for (BasicBlock *BB : ExitBlocks)
      if (!DT.dominates(I->getParent(), BB))
        return false;
    
    return true;
  }

  /**
   * Restituisce true se l'istruzione I è morta al di fuori del loop L. Se 
   * tutti gli user di I sono all'interno del loop, allora I è morta al di 
   * fuori del loop.
   */
  bool FilterCandidateAnalysis::isVariableDeadOutsideLoop(
    Instruction *I, Loop &L) 
  {
    for (auto *U : I->users())
      if (Instruction *UserInst = dyn_cast<Instruction>(U))
        if (!L.contains(UserInst->getParent()))
          return false;
    return true;
  }
  
  /**
   * Se l'istruzione I è utilizzata da una phi all'interno del loop, 
   * controlla dove sono definite le reaching definitions della phi. 
   * Se sono tutte esterne al loop, allora I è assegnato una sola volta. 
   * Altrimenti, se anche solo una p una costante oppure una definizione 
   * interna al loop, allora I è assegnato più di una volta.
   */
  bool FilterCandidateAnalysis::isValueAssignedOnce(Instruction *I, Loop &L) {

    for (User *User : I->users()) {

      if (isa<PHINode>(User) && L.contains(dyn_cast<Instruction>(User))) {
        PHINode *phi = dyn_cast<PHINode>(User);
        
        for (unsigned j = 0; j < phi->getNumIncomingValues(); ++j) {
          Value *ReachingDef = phi->getIncomingValue(j);
          
          if (isa<Constant>(ReachingDef))
            return false;
          
          if (Instruction *ReachingDefInst = dyn_cast<Instruction>(ReachingDef))
            if (ReachingDefInst != I && L.contains(ReachingDefInst))
              return false;
        }
      }
    }

    return true;
  }
  
  /**
   * Restituisce true se l'istruzione I è definita prima di essere usata.
   */
  bool FilterCandidateAnalysis::isDefinedBeforeUse(
    Instruction *I, Loop &L, DominatorTree &DT) 
  {
    BasicBlock *PBB = I->getParent(); 
    
    for (User *U : I->users()) {
      Instruction *UserInst = dyn_cast<Instruction>(U);
      BasicBlock *UBB = UserInst->getParent(); 

      if (L.contains(UBB)) {
        if (PBB == UBB) {
          if (UserInst->comesBefore(I))
            return false;
        } else {
          if (!DT.dominates(PBB, UBB))
            return false;
        }
      }
      
    }

    return true;
  }

  /**
   * Filtra le istruzioni loop-invariant per determinare se sono
   * candidate a essere spostate all'esterno del loop.
   */
  void FilterCandidateAnalysis::filterInvariantInstructions(
    Loop &L, DominatorTree &DT, 
    std::set<Instruction*> &invariantInstructionSet, 
    std::set<Instruction*> &candidateInstructionSet) 
  {
    for (Instruction* I : invariantInstructionSet) {
      
      bool assignedOnce     = \
        FilterCandidateAnalysis::isValueAssignedOnce(I, L);
      bool definedBeforeUse = \
        FilterCandidateAnalysis::isDefinedBeforeUse(I, L, DT);
      bool dominatesExits   = \
        FilterCandidateAnalysis::instructionDominatesAllExits(I, L, DT);
      bool deadOutsideLoop  = \
        FilterCandidateAnalysis::isVariableDeadOutsideLoop(I, L);

      bool isCandidate = assignedOnce && definedBeforeUse && 
                        (dominatesExits || deadOutsideLoop);

      if (isCandidate) {
        candidateInstructionSet.insert(I);
        
        utils::printInstruction(
          "\033[1;38:5:214m[LICM-FILTERING] Movable Instruction:\033[0m\t"
          "\033[0;38:5:214m", I
        );  // DEBUG
      }
    }
  }

} // namespace graboidpasses::licm