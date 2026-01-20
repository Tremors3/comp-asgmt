
//==============================================================================
// FILE:
//    LFPassManager.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#include "LFPassManager.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  /**
   * Entry point.
   */
  void LFPassManager::startPass() {
    iterateAndFuseLoops();
  }

  void LFPassManager::iterateAndFuseLoops() {
    utils::debug("\n\033[1;38:5:15m "
                 ">============================================================"
                 "========<\033[0m");
    utils::debug("\n[LF] Run on function: " + F.getName().str(), utils::GREEN);

    // Raggruppa gli innermost loop per loop parent
    DenseMap<Loop *, SmallVector<Loop *, 4>> LoopsByParent;

    for (Loop *TopLevelLoop : LI) {
      for (Loop *L : depth_first(TopLevelLoop)) {
        if (!L->isInnermost())
          continue;

        // If parent == nullptr: top level loop
        Loop *Parent = L->getParentLoop();
        LoopsByParent[Parent].push_back(L);
      }
    }

    onePerformed = false;

    // Processa gli innermost loops per loop parent
    for (auto &[Parent, Loops] : LoopsByParent) {
      if (Loops.size() < 2)
        continue;

      // Analizza coppie di loop allo stesso livello
      for (unsigned i = Loops.size() - 1; i > 0; --i) {
        // I loop che condividono lo stesso parent loop
        // si trovano sullo stesso livello di annidamento
        Loop *First  = Loops[i];
        Loop *Second = Loops[i - 1];

        bool IsValid = analyzeCouple(First, Second);

        if (!IsValid)
          continue;

        utils::debug("[LF] Valid couple found.", utils::PURPLE);

        LoopFusion lf(First, Second, LI, F);
        lf.fuse();

        utils::debug("[LF] Couple fused successfully.", utils::PURPLE);

        outs() << '\n';
        F.print(outs());
        outs() << '\n';

        onePerformed = true;
      }
    }
  }

  /**
   * Analyzes two loops and check if they are adjacent, control flow equivalent,
   * trip count equivalent and if there are any negative distance dependencies
   * between them. Both loops must be in simplify and rotated form.
   */
  bool LFPassManager::analyzeCouple(Loop *FL, Loop *SL) const {

    utils::debug("[LF] 1° loop, depth: " + std::to_string(FL->getLoopDepth()),
                 utils::WHITE);
    utils::debug("[LF] 2° loop, depth: " + std::to_string(SL->getLoopDepth()),
                 utils::WHITE);

    if (!FL || !SL) {
      utils::debug("[LF] At least one of the two loops is null.",
                   utils::YELLOW);
      return false;
    }

    if (!FL->isLoopSimplifyForm() || !SL->isLoopSimplifyForm()) {
      utils::debug("[LF] At least one of the two loops isn't in simplify form.",
                   utils::YELLOW);
      return false;
    }

    if (!FL->isRotatedForm() || !SL->isRotatedForm()) {
      utils::debug("[LF] At least one of the two loops isn't in rotated form.",
                   utils::YELLOW);
      return false;
    }

    if (FL->getLoopDepth() != SL->getLoopDepth()) {
      utils::debug("[LF] Loops at different depth.", utils::YELLOW);
      return false;
    }

    AdjacentAnalysis aa(FL, SL);
    TripCountEquivalenceAnalysis tce(FL, SL, SE);
    ControlFlowEquivalenceAnalysis cfe(FL, SL, DT, PDT);
    NegativeDistanceDependenceAnalysis ndd(FL, SL, DI, SE);
    
    bool isCoupleAdjacent = aa.analyse();
    utils::debugYesNo("[LF-ADJ] Is couple Adjacent?:\t", isCoupleAdjacent);
    
    bool isCoupleCFE = cfe.analyse();
    utils::debugYesNo("[LF-CFE] Is couple CFE?: \t", isCoupleCFE);
    
    bool isCoupleTCE = tce.analyse();
    utils::debugYesNo("[LF-TCE] Is couple TCE?: \t", isCoupleTCE);

    bool isCoupleNotNDD = !ndd.analyse();
    utils::debugYesNo("[LF-NDD] Couple isn't NDD: \t", isCoupleNotNDD);

    return isCoupleAdjacent && isCoupleCFE && isCoupleTCE && isCoupleNotNDD;
  }

} // namespace graboidpasses::lf