//============================================================================//
// FILE:
//    PassRegister.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion.
//
// EXECUTE WITH PASSES:
//    opt ... -passes="loop(loop-rotate),function(gb-loop-fusion)" ...
//
// License: GPL3
//============================================================================//
#include <llvm/ADT/DepthFirstIterator.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/DependenceAnalysis.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/Casting.h>
#include <vector>

#include "Utils.hpp"

using namespace llvm;

/*

### PUNTO PRIMO - ADJACENCY

1. Se il primo loop è guarded allora non possiamo fonderlo con un secondo senza
guard perchè il corpo del secondo loop non viene per forza saltato se viene
saltato il corpo del primo loop.

2. Possiamo fondere i loop solamente in due casi specifici:
    - se entrambi i loop sono guarded e le guard sono tra loro equivalenti
(condividono la stessa condizione).
    - se entrambi i loop non sono guarded.

3. I casi particolari in cui dobbiamo verificare la presenza di istruzioni ("in
più") rompiscatole sono:
    - quando il preheader del secondo loop non coincide con l'exit block del
primo l'oop; in questo caso dobbiamo verificare la presenza di istruzioni che
non possono essere spostate fuori dai piedi.
    - NOTA: Quindi non dobbiamo iterare ricorsivamente i blocchi.

4. Il prof non richiede lo spostamento delle istruzioni.

TODO: Capire perche' la prima guard non punta alla seconda in alcuni casi (i.e.
il penultimo dei casi). O la prima punta a qualcos'altro o quando si prende la
guard dal secondo loop non si ottiene quella che dovrebbe essere la guard.

*/

namespace graboidpasses::lf {

struct LoopFusionPass : PassInfoMixin<LoopFusionPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    return (runOnFunction(F, AM) ? PreservedAnalyses::none()
                                 : PreservedAnalyses::all());
  }

  static bool isRequired() { return true; }

private:
  /* ------------------------------------------------------------------------ */

  /**
   * Returns the guard of the loop if it exists
   */
  BasicBlock *getLoopGuard(Loop *L) const {
    if (BranchInst *br = L->getLoopGuardBranch())
      return br->getParent();
    return nullptr;
  }

  /**
   * Checks for unwanted instructions inside the preheader
   */
  bool cleanFromUnwantedInstructions(BasicBlock *BB,
                                     std::set<Instruction *> whitelist) const {
    for (Instruction &I : *BB)
      if (whitelist.count(&I) != 1)
        return false;
    return true;
  }

  /**
   * Check if the first guard points to the second guard
   */
  bool firstGuardPointsSecondGuard(BasicBlock *firGuard,
                                   BasicBlock *secGuard) const {
    // If the first guard doesn't point to the second one
    // we are sure there are basic blocks between the
    // first loop exit and the second loop guard.
    for (auto FG_Successor : successors(firGuard))
      if (secGuard == FG_Successor)
        return true;
    return false;
  }

  /**
   * Get the guard branch condition
   */
  Value *getGuardBranchCondition(BasicBlock *BB) const {
    if (auto *branchInst = dyn_cast<BranchInst>(BB->getTerminator()))
      if (branchInst->isConditional())
        return branchInst->getCondition();
    return nullptr;
  }

  /**
   * Check if both guard condition are equal by comparing every component
   * of the condition
   */
  bool areGuardsEquivalent(BasicBlock *firGuard, BasicBlock *secGuard) const {
    if (auto *firCond = dyn_cast<ICmpInst>(getGuardBranchCondition(firGuard))) {
      if (auto *secCond =
              dyn_cast<ICmpInst>(getGuardBranchCondition(secGuard))) {
        if (firCond->getPredicate() == secCond->getPredicate() &&
            firCond->getOperand(0) == secCond->getOperand(0) &&
            firCond->getOperand(1) == secCond->getOperand(1)) {
          utils::debug("[LF-ADJ] Both guards have the same condition.",
                       utils::DARK_GREEN);
          return true;
        }
      }
    }
    return false;
  }

  /* ------------------------------------------------------------------------ */

  bool checkGuardedLoopsAdjacency(Loop *firstLoop, Loop *secondLoop,
                                  BasicBlock *firstGuard,
                                  BasicBlock *secondGuard) const {

    // VALIDATING

    // First loop ExitBlock validation
    BasicBlock *FL_exitBlock = firstLoop->getExitBlock();
    if (!FL_exitBlock) {
      utils::debug("[LF-ADJ] First loop has multiple exit blocks.",
                   utils::YELLOW);
      return false;
    }

    // Second loop Preheader validation
    BasicBlock *SL_preheader = secondLoop->getLoopPreheader();
    if (!SL_preheader) {
      utils::debug("[LF-ADJ] Second loop does not have a preheader.",
                   utils::YELLOW);
      return false;
    }

    // CHECKING FOR THE ABSENCE OF OTHER BASIC BLOCKS

    if (!firstGuardPointsSecondGuard(firstGuard, secondGuard)) {
      utils::debug("[LF-ADJ] The first guard doesn't point to the second one.",
                   utils::YELLOW);
      return false;
    }

    // CHECKING FOR UNWANTED INSTRUCTIONS

    // Checking for unwanted instructions in the first loop exit block
    if (!cleanFromUnwantedInstructions(SL_preheader,
                                       {SL_preheader->getTerminator()})) {
      utils::debug("[LF-ADJ] Preheader not clean.", utils::YELLOW);
      return false;
    }

    // Checking for unwanted instructions in the second loop preheader
    if (!cleanFromUnwantedInstructions(FL_exitBlock,
                                       {FL_exitBlock->getTerminator()})) {
      utils::debug("[LF-ADJ] Exit block not clean.", utils::YELLOW);
      return false;
    }

    // Checking for unwanted instructions in the second loop guard
    if (!cleanFromUnwantedInstructions(
            secondGuard,
            {secondGuard->getTerminator(),
             dyn_cast<Instruction>(getGuardBranchCondition(secondGuard))})) {
      utils::debug("[LF-ADJ] Second guard not clean.", utils::YELLOW);
      return false;
    }

    // CHECKING FOR GUARDS EQUIVALENCE

    if (!areGuardsEquivalent(firstGuard, secondGuard)) {
      utils::debug("[LF-ADJ] Guards are not equivalents.", utils::YELLOW);
      return false;
    }

    return true;
  }

  /**
   * Check if the first loop is adjacent to the second one and if they are clean
   * so that they don't contain unwanted instructions.
   */
  bool checkUnguardedLoopsAdjacency(Loop *firstLoop, Loop *secondLoop) const {

    // VALIDATING

    // First loop ExitBlock validation
    BasicBlock *FL_exitBlock = firstLoop->getExitBlock();
    if (!FL_exitBlock) {
      utils::debug("[LF-ADJ] First loop has multiple exit blocks.",
                   utils::YELLOW);
      return false;
    }

    // Second loop Preheader validation
    BasicBlock *SL_preheader = secondLoop->getLoopPreheader();
    if (!SL_preheader) {
      utils::debug("[LF-ADJ] Second loop does not have a preheader.",
                   utils::YELLOW);
      return false;
    }

    // CHECKING FOR THE PRESENCE OF OTHER BASIC BLOCKS

    // Checking for the presence of basic blocks between the exit and preheader
    if (FL_exitBlock != SL_preheader) {
      utils::debug("[LF-ADJ] The exit block isn't the preheader.",
                   utils::YELLOW);
      return false;
    }

    // CHECKING FOR UNWANTED INSTRUCTIONS

    // Checking for unwanted instructions in the first loop exit block
    if (!cleanFromUnwantedInstructions(SL_preheader,
                                       {SL_preheader->getTerminator()})) {
      utils::debug("[LF-ADJ] Preheader not clean.", utils::YELLOW);
      return false;
    }

    return true;
  }

  /**
   * Runs adjacency analysis
   */
  bool adjacentAnalysis(Loop *firstLoop, Loop *secondLoop) const {

    BasicBlock *firstGuard = getLoopGuard(firstLoop),
               *secondGuard = getLoopGuard(secondLoop);

    utils::debugYesNo("[LF-ADJ] First Loop guarded?: \t", firstGuard);
    utils::debugYesNo("[LF-ADJ] Second Loop guarded?:\t", secondGuard);

    // both are unguarded
    if (firstGuard == nullptr && secondGuard == nullptr) {
      utils::debug("[LF-ADJ] Both are unguarded.", utils::BLUE);
      return checkUnguardedLoopsAdjacency(firstLoop, secondLoop);
    }

    // both are guarded
    if (firstGuard != nullptr && secondGuard != nullptr) {
      utils::debug("[LF-ADJ] Both are guarded.", utils::BLUE);
      return checkGuardedLoopsAdjacency(firstLoop, secondLoop, firstGuard,
                                        secondGuard);
    }

    // one is guarded
    utils::debug("[LF-ADJ] Only one is guarded.", utils::YELLOW);
    return false;
  }

  /* ------------------------------------------------------------------------ */

  /**
   * Checks if the two loops are Control Flow Equivalent.
   */
  bool areControlFlowEquivalent(Loop *firstLoop, Loop *secondLoop,
                                DominatorTree *DT,
                                PostDominatorTree *PDT) const {

    BasicBlock *firstEntry = getLoopGuard(firstLoop),
               *secondEntry = getLoopGuard(secondLoop);

    // If not all the loops are guarded, use the header instead
    if (firstEntry == nullptr || secondEntry == nullptr) {
      firstEntry = firstLoop->getHeader();
      secondEntry = secondLoop->getHeader();
    }

    return (DT->dominates(firstEntry, secondEntry) &&
            PDT->dominates(secondEntry, firstEntry));
  }

  /* ------------------------------------------------------------------------ */

  bool areTripCountEquivalent(Loop *firstLoop, Loop *secondLoop,
                              ScalarEvolution *SE) const {
    // https://llvm.org/devmtg/2018-04/slides/Absar-ScalarEvolution.pdf

    if (!SE->hasLoopInvariantBackedgeTakenCount(firstLoop) ||
        !SE->hasLoopInvariantBackedgeTakenCount(secondLoop)) {
      utils::debug("[LF-TCE] At least one of the loops hasn't got a loop "
                   "invariant backedge.",
                   utils::YELLOW);
      return false;
    }

    // https://llvm.org/docs/LoopTerminology.html
    const SCEV *l1scev = SE->getBackedgeTakenCount(firstLoop),
               *l2scev = SE->getBackedgeTakenCount(secondLoop);

    if (isa<SCEVCouldNotCompute>(l1scev) || isa<SCEVCouldNotCompute>(l2scev)) {
      utils::debug("[LF-TCE] The backedge is not of the correct type.",
                   utils::YELLOW);
      return false;
    }

    if (l1scev != l2scev) {

      unsigned t1 = SE->getSmallConstantTripMultiple(firstLoop, l1scev),
               t2 = SE->getSmallConstantTripMultiple(secondLoop, l2scev);

      utils::debug("[LF-TCE] The trip count of the two loops is different: (" +
                       std::to_string(t1) + " != " + std::to_string(t2) + ").",
                   utils::YELLOW);

      return false;
    }

    return true;
  }

  /* ------------------------------------------------------------------------ */

  void getLoopInstructionByType(Loop *L, std::vector<Instruction *> *stores,
                                std::vector<Instruction *> *loads) const {
    for (BasicBlock *BB : L->getBlocks()) {
      for (Instruction &I : *BB) {
        if (isa<StoreInst>(I))
          stores->push_back(&I);

        else if (isa<LoadInst>(I))
          loads->push_back(&I);
      }
    }
  }

  bool areNegativeDistanceDependent(Loop *firstLoop, Loop *secondLoop,
                                    DependenceInfo *DI) const {

    std::vector<Instruction *> FirstLoopStoreInst, FirstLoopLoadInst,
        SecondLoopStoreInst, SecondLoopLoadInst;

    getLoopInstructionByType(firstLoop, &FirstLoopStoreInst,
                             &FirstLoopLoadInst);
    getLoopInstructionByType(secondLoop, &SecondLoopStoreInst,
                             &SecondLoopLoadInst);

    for (Instruction *Store : FirstLoopStoreInst)
      for (Instruction *Load : SecondLoopLoadInst)
        if (DI->depends(Store, Load, true))
          return true;

    for (Instruction *Load : FirstLoopLoadInst)
      for (Instruction *Store : SecondLoopStoreInst)
        if (DI->depends(Store, Load, true))
          return true;

    return false;
  }

  /* ------------------------------------------------------------------------ */

  /**
   * Analyze two loops
   */
  bool analyzeCouple(Loop *FL, Loop *SL, DominatorTree *DT,
                     PostDominatorTree *PDT, ScalarEvolution *SE,
                     DependenceInfo *DI) const {
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

    bool isCoupleAdjacent = adjacentAnalysis(FL, SL);
    utils::debugYesNo("[LF-ADJ] Is couple Adjacent?:\t", isCoupleAdjacent);

    bool isCoupleCFE = areControlFlowEquivalent(FL, SL, DT, PDT);
    utils::debugYesNo("[LF-CFE] Is couple CFE?: \t", isCoupleCFE);

    bool isCoupleTCE = areTripCountEquivalent(FL, SL, SE);
    utils::debugYesNo("[LF-TCE] Is couple TCE?: \t", isCoupleTCE);

    bool isCoupleNotNDD = !areNegativeDistanceDependent(FL, SL, DI);
    utils::debugYesNo("[LF-NDD] Couple is't NDD: \t", isCoupleNotNDD);

    return isCoupleAdjacent && isCoupleCFE && isCoupleTCE && isCoupleNotNDD;
  }

  /* ------------------------------------------------------------------------ */

  bool fuseLoops(Loop *firstLoop, Loop *secondLoop) { return false; }

  /* ------------------------------------------------------------------------ */

  bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
    utils::debug("\n\033[1;38:5:15m "
                 ">============================================================"
                 "=<\033[0m");
    utils::debug("\n[LF] Run on function: " + F.getName().str(), utils::GREEN);

    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
    ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
    DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

    SmallVector<Loop *, 8> Worklist;

    for (Loop *TopLevelLoop : LI)
      for (Loop *L : depth_first(TopLevelLoop))
        if (L->isInnermost())
          Worklist.push_back(L);

    if (Worklist.size() < 2) {
      utils::debug("[LF] Less than 2 top-level loops in function.",
                   utils::YELLOW);
      return false;
    }

    bool Transformed = false;

    for (unsigned long i = Worklist.size() - 1; i > 0; --i) {

      bool isCoupleValid =
          analyzeCouple(Worklist[i], Worklist[i - 1], &DT, &PDT, &SE, &DI);

      if (isCoupleValid)
        utils::debug("[LF] Couple is valid.", utils::PURPLE);

      bool isCoupleFused =
          isCoupleValid && fuseLoops(Worklist[i], Worklist[i - 1]);

      if (isCoupleFused)
        utils::debug("[LF] Couple is fused.", utils::PURPLE);

      Transformed |= isCoupleFused;
    }

    return Transformed;
  }
};

} // namespace graboidpasses::lf

llvm::PassPluginLibraryInfo getPassRegisterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassRegister", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "gb-loop-fusion") {
                    FPM.addPass(graboidpasses::lf::LoopFusionPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getPassRegisterPluginInfo();
}
