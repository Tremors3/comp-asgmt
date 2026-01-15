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
   * Entry point.
   */
  void LicmPassManager::startPass() {
    return iterateTopLevelLoops();
  }

  /**
   * Visita sequenzialmente i loop di livello più elevato (top level loops).
   */
  void LicmPassManager::iterateTopLevelLoops() {
    for (auto &L : loopinfo->getTopLevelLoops())
      iterateSubLevelLoops(*L);
  }

  /**
   * Visita ricorsivamente (DFS Postorder) i loops annidati (sub loops).
   */
  void LicmPassManager::iterateSubLevelLoops(Loop &L) {
    for (auto &nested : L.getSubLoops())
      iterateSubLevelLoops(*nested);
    processLoop(L);
  }

  /**
   * Gestisce una pipeline di tre operazioni:
   * Analizza le istruzioni del loop per determinare quali sono loop-invariant.
   * Filtra le istruzioni candidate allo spostamento tra quelle loop-invariant.
   * Effettua lo spostamento delle istruzioni filtrate nel preheader del loop.
   */
  void LicmPassManager::processLoop(Loop &L) {
    outs() << "\033[1;38:5:255m[LICM] Run on loop at:\033[0m "
           << "\033[0;38:5:255m" << L.getLoopID()
           << ", Depth: " << L.getLoopDepth() << "\033[0m\n";  // DEBUG

    InstrMap candidateInstructions, invariantInstructions;

    // 1. Analisi loop-invarianti:
    //    Costruisce la gerarchia delle istruzioni invarianti
    LoopInvariantAnalysis lia(&L);
    lia.analyzeLoop();
    invariantInstructions = lia.getInvariants();

    // 2. Filtraggio:
    //    Seleziona tra le invarianti quelle realmente spostabili
    if (!invariantInstructions.empty()) {
      FilterCandidateAnalysis fca(&L, domtree, &invariantInstructions);
      fca.filterCandidates();
      candidateInstructions = fca.getCandidates();
    }

    // 3. Code motion:
    //    Sposta le istruzioni candidate nel preheader rispettando le dipendenze
    if (!candidateInstructions.empty()) {
      CodeMotion cm(&L, &candidateInstructions);
      cm.executeMotion();

      // Segnala se è stata effettuata almeno una trasformazione
      if(cm.hasOneMotionPerformed())
        setTransformed();
    }

    outs() << '\n';
  }

} // namespace graboidpasses::licm
