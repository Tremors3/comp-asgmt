//==============================================================================
// FILE:
//    PassRegister.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion.
//
// License: GPL3
//==============================================================================
#include "llvm/Analysis/PostDominators.h"
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/TrackingMDRef.h>
#include <llvm/IR/Value.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

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
  bool containsUnwantedInstructions(BasicBlock *BB,
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
    // we are shure there are basic blocks between the
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
          utils::debug("[LF-ADJ] Both guards have the same condition");
          return true;
        }
      }
    }
    return false;
  }

  /**
   * Check if the first loop is adjacent to the second one and if they are clean
   * so that they don't contain unwanted instructions.
   */
  bool checkUnguardedLoopsAdjacency(Loop *FirstLoop, Loop *SecondLoop) const {

    // First loop ExitBlock validation
    BasicBlock *exitBlock = FirstLoop->getExitBlock();
    if (!exitBlock) {
      utils::debug("[LF-ADJ] First loop has multiple exit blocks.");
      return false;
    }

    // First loop ExitBlock Successor validation
    BasicBlock *FL_ExitSucc = exitBlock->getSingleSuccessor();
    if (!FL_ExitSucc) {
      utils::debug("[LF-ADJ] First loop does not have a exit successor.");
      return false;
    }

    // Second loop Preheader validation
    BasicBlock *SL_Preheader = SecondLoop->getLoopPreheader();
    if (!SL_Preheader) {
      utils::debug("[LF-ADJ] Second loop does not have a preheader.");
      return false;
    }

    // Checking for unwanted instructions --------------------------------------

    // Checking for unwanted instructions in the exit block
    if (!containsUnwantedInstructions(SL_Preheader,
                                      {SL_Preheader->getTerminator()})) {
      utils::debug("[LF-ADJ] Preheader not clean.");
      return false;
    }

    // Checking for unwanted instructions in the preheader
    if ((exitBlock != SL_Preheader) &&
        !containsUnwantedInstructions(exitBlock,
                                      {exitBlock->getTerminator()})) {
      utils::debug("[LF-ADJ] Exit block not clean.");
      return false;
    }

    // Checking for the absence of other basic blocks --------------------------

    // Checking for the absence of other basic blocks between the exit of the
    // first loop and the preheader of the second one
    if (exitBlock == SL_Preheader || FL_ExitSucc == SL_Preheader)
      return true;

    return false;
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
     utils::debug("[LF-ADJ] Both are unguarded");
     return checkUnguardedLoopsAdjacency(firstLoop, secondLoop);
   }
    // both are guarded
    if (firstGuard != nullptr && secondGuard != nullptr) {
      utils::debug("[LF-ADJ] Both are guarded");
      return firstGuardPointsSecondGuard(firstGuard, secondGuard) &&
             areGuardsEquivalent(firstGuard, secondGuard) &&
             containsUnwantedInstructions(
                 secondGuard, {secondGuard->getTerminator(),
                               dyn_cast<Instruction>(
                                   getGuardBranchCondition(secondGuard))}) &&
             containsUnwantedInstructions(
                 secondLoop->getLoopPreheader(),
                 {secondLoop->getLoopPreheader()->getTerminator()});
    }

    // one is guarded
    utils::debug("[LF-ADJ] Only one is guarded");
    return false;
  }

  /**
   * Checks if the two loops are Control Flow Equivalent.
   */
  bool areControlFlowEquivalent(Loop *firstLoop, Loop *secondLoop,
                                DominatorTree *DT,
                                PostDominatorTree *PDT) const {
    return (DT->dominates(firstLoop->getHeader(), secondLoop->getHeader()) &&
            PDT->dominates(secondLoop->getHeader(), firstLoop->getHeader()));
  }

  /**
   * Analyze two loops
   */
  bool analyzeCouple(Loop *FL, Loop *SL, DominatorTree *DT,
                     PostDominatorTree *PDT) const {
    utils::debug("[LF] Couple:", "\033[1;38:5:255m", "\033[0m");
    utils::debug("[LF] 1° loop, depth: " + std::to_string(FL->getLoopDepth()),
                 "\033[1;38:5:255m", "\033[0m");
    utils::debug("[LF] 2° loop, depth: " + std::to_string(SL->getLoopDepth()),
                 "\033[1;38:5:255m", "\033[0m");

    if (!FL || !SL) {
      utils::debug("[LF] At least one of the two loops is null");
      return false;
    }

    if (!FL->isLoopSimplifyForm() || !SL->isLoopSimplifyForm()) {
      utils::debug("[LF] At least one of the two loops isn't in simplify form");
      return false;
    }

    bool isCoupleAdjacent = adjacentAnalysis(FL, SL);
    utils::debugYesNo("[LF-ADJ] Is couple Adjacent?:\t", isCoupleAdjacent);

    bool isCoupleCFE = areControlFlowEquivalent(FL, SL, DT, PDT);
    utils::debugYesNo("[LF-CFE] Is couple CFE?: \t", isCoupleCFE);

    return isCoupleAdjacent && isCoupleCFE;
  }

  bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
    utils::debug("\n[LF] Run on function: " + F.getName().str(),
                 "\033[1;38:5:40m", "\033[0m");

    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);

    const std::vector<Loop *> LoopVect = LI.getTopLevelLoops();

    if (LoopVect.size() < 2) {
      utils::debug("[LF] Less than 2 top-level loops in function.");
      return false;
    }

    Loop *FL = *(++LoopVect.begin());
    Loop *SL = *LoopVect.begin();

    bool isCoupleValid = analyzeCouple(FL, SL, &DT, &PDT);

    return false;
  }
};

} // namespace graboidpasses::lf

llvm::PassPluginLibraryInfo getPassRegisterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassRegister", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "-loop-fusion-pass") {
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
