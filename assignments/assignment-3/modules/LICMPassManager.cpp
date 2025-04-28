//==============================================================================
// FILE:
//    LICMPassManager.cpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#include "LICMPassManager.hpp"
#include "LoopInvariantAnalysis.hpp"
#include "FilterCandidateAnalysis.hpp"
#include "CodeMotion.hpp"

using namespace llvm;

namespace graboidpasses::licm {

  /**
   * Per ciascun loop, esegue l'analisi per determinare le istruzioni
   * loop-invariant. In seguito filtra le istruzioni loop-invariant
   * per determinare se sono candidate a essere spostate all'esterno
   * del loop.
   */
  bool LicmPassManager::processLoop(Loop &L, DominatorTree &DT) {
      outs() << "\033[1;38:5:255m[LICM] Run on loop at:\033[0m "
              << "\033[0;38:5:255m" << L.getLoopID()
              << ", Depth: " << L.getLoopDepth() << "\033[0m\n";  // DEBUG
  
      std::set<Instruction*> candidateInstructionSet, invariantInstructionSet;
      
      LoopInvariantAnalysis lia;
      lia.markLoopInvariantInstructions(L, invariantInstructionSet);

      if (invariantInstructionSet.empty())
        return false;
      
      FilterCandidateAnalysis fca;
      fca.filterInvariantInstructions(
        L, DT, invariantInstructionSet, candidateInstructionSet);
      
      if (candidateInstructionSet.empty())
        return false;

      CodeMotion cm;
      cm.moveInstructions(L, candidateInstructionSet);
      
      outs() << '\n';

      return true;
  }
  
  /**
   * Visita ricorsiva (DFS-Postorder) sui loops annidati (sub-loops).
   */
  bool LicmPassManager::iterateSubLevelLoops(Loop &L, DominatorTree &DT) {
    bool Transformed = false;

    for (auto &nested : L.getSubLoops())
      Transformed |= LicmPassManager::iterateSubLevelLoops(*nested, DT);
    
    Transformed |= LicmPassManager::processLoop(L, DT);
    
    return Transformed;
  }
  
  /**
   * Visita iterativa (sequenziale) sui loops di livello più alto (top-loops).
   */
  bool LicmPassManager::iterateTopLevelLoops(LoopInfo &LI, DominatorTree &DT) {
    bool Transformed = false;

    for (auto &L : LI.getTopLevelLoops())
      Transformed |= LicmPassManager::iterateSubLevelLoops(*L, DT);

    return Transformed;
  }

  /*------------------------------------------------------------------------*/

  bool LicmPassManager::runOnFunction(Function &F, FunctionAnalysisManager &AM){
    outs() << "\033[1;38:5:40m[LICM] Run on function:\033[0m "
            << "\033[0;38:5:40m" << F.getName() << "\033[0m\n";  // DEBUG
    
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    
    return LicmPassManager::iterateTopLevelLoops(LI, DT);
  }

  PreservedAnalyses LicmPassManager::run(
    Function &F, FunctionAnalysisManager &AM) 
  {
    return (LicmPassManager::runOnFunction(F, AM) ? 
      PreservedAnalyses::none() : PreservedAnalyses::all());
  }

} // namespace graboidpasses::licm