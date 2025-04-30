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

  void LicmPassManager::startPass() {
    return iterateTopLevelLoops();
  }

  /**
   * Visita iterativa (sequenziale) sui loops di livello più alto (top-loops).
   */
  void LicmPassManager::iterateTopLevelLoops() {
    for (auto &L : loopinfo->getTopLevelLoops())
      iterateSubLevelLoops(*L);
  }

  /**
   * Visita ricorsiva (DFS-Postorder) sui loops annidati (sub-loops).
   */
  void LicmPassManager::iterateSubLevelLoops(Loop &L) {
    for (auto &nested : L.getSubLoops())
      iterateSubLevelLoops(*nested);
    processLoop(L);
  }

  /**
   * Per ciascun loop, esegue l'analisi per determinare le istruzioni
   * loop-invariant. In seguito filtra le istruzioni loop-invariant
   * per determinare se sono candidate a essere spostate all'esterno
   * del loop.
   */
  void LicmPassManager::processLoop(Loop &L) {
    outs() << "\033[1;38:5:255m[LICM] Run on loop at:\033[0m "
           << "\033[0;38:5:255m" << L.getLoopID()
           << ", Depth: " << L.getLoopDepth() << "\033[0m\n";  // DEBUG

    std::set<Instruction*> candidateInstructions, invariantInstructions;

    LoopInvariantAnalysis lia(&L);
    lia.analyzeLoop();
    invariantInstructions = lia.getInvariantInstructions();

    if (!invariantInstructions.empty()) {
      FilterCandidateAnalysis fca(&L, domtree, &invariantInstructions);
      fca.filterCandidates();
      candidateInstructions = fca.getCandidates();
    }

    if (!candidateInstructions.empty()) {
      CodeMotion cm(&L, &candidateInstructions);
      cm.executeMotion();
      if(cm.hasOneMotionPerformed())
        setTransformed();
    }

    outs() << '\n';
  }

} // namespace graboidpasses::licm
