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
#include "llvm/IR/Dominators.h"
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/TrackingMDRef.h>
#include <llvm/IR/Value.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>

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
  bool isLoopExitBlock(Loop *L, BasicBlock *ExitToCheck) const {
    SmallVector<BasicBlock *, 1> ExitBlocks;
    L->getExitBlocks(ExitBlocks);
    for (BasicBlock *Exit : ExitBlocks)
      if (ExitToCheck == Exit)
        return true;
    return false;
  }

  bool checkGuardSuccessors(Loop *L, BasicBlock *First, BasicBlock *Second,
                            BasicBlock *EntryBlock) const {
    // Checking that one of the branch successors is the "entry" block
    // (header/preheader) of the loop. And the other one is an exit
    // block of the loop.
    return (First == EntryBlock &&
            isLoopExitBlock(L, Second)); // Second == L->getExitBlock()
  }

  BasicBlock *customLoopGuardRetrieval(Loop *L) const {
    // Important: The function does not assume that the loop is in simplified
    // form

    // Getting the preheader, if it exists. If it there isn't, use the header.
    // Note: a loop can be guarded even if it doesn't have a preheader.
    BasicBlock *EntryBlock = L->getLoopPreheader();
    if (EntryBlock == nullptr)
      EntryBlock = L->getHeader();

    // We count how many external predecessors the "entry" block
    // (header/preheader) has. To be guarded, a loop must have exactly one
    // external predecessor.
    unsigned externalPreds = 0;

    for (BasicBlock *pred : predecessors(EntryBlock))
      if (!L->contains(pred))
        externalPreds++;

    if (externalPreds != 1)
      return nullptr;

    // Checking the predecessors of the loop entry block (header/preheader).
    for (BasicBlock *pred : predecessors(EntryBlock)) {
      if (L->contains(pred))
        continue; // the predecessor is inside the loop (a latch).

      // If the external predecessor ends with a conditional branch instruction
      // it could be a guard.
      if (BranchInst *br = dyn_cast<BranchInst>(pred->getTerminator())) {

        if (!br->isConditional())
          continue; // the guard branch inst must be conditional

        // One of the successors must redirect to the header block,
        // the other one must go at the exit of the current loop.
        BasicBlock *succ1 = br->getSuccessor(0);
        BasicBlock *succ2 = br->getSuccessor(1);

        // Checking that one successor points to the loop "entry" block
        // (header/preheader). And the second successor points at an
        // exit block.
        if (checkGuardSuccessors(L, succ1, succ2, EntryBlock) ||
            checkGuardSuccessors(L, succ2, succ1, EntryBlock))
          return pred;
      }
    }

    return nullptr;
  }

  /**
   * Returns the guard of the loop if it exists
   */
  BasicBlock *getLoopGuard(Loop *L, bool custom = false) const {

    if (custom)
      // Using custom guard getter
      return customLoopGuardRetrieval(L);

    if (BranchInst *br = L->getLoopGuardBranch())
      // Using predefined guard getter
      return br->getParent();

    return nullptr;
  }

  /**
   * Check if the first loop is adjacent to the second one
   */
  bool isFirstNextToSecond(Loop *FirstLoop, Loop *SecondLoop) {
    BasicBlock *FL_ExitSucc = FirstLoop->getExitBlock()->getNextNode();
    BasicBlock *SL_Preheader = SecondLoop->getLoopPreheader();

    // Case 1:  1st loop exit successor == 2nd loop header
    if (FL_ExitSucc == SecondLoop->getHeader())
      return true;

    // Case 2:  1st loop exit successor == 2nd loop preheader
    if (FL_ExitSucc == SL_Preheader) {

      // Checking for unwanted instructions in the preheader
      for (Instruction &I : *SL_Preheader)
        if (&I != SL_Preheader->getTerminator())
          return false;

      return true;
    }

    // Case 3:  1st loop exit successor == another block
    return false;
  }

  /**
   * Check if the first guard points to the second guard
   */
  bool firstGuardPointsSecondGuard(BasicBlock *firGuard, BasicBlock *secGuard) {
    for (auto FG_Successor : successors(firGuard)) {
      if (secGuard == FG_Successor)
        return true;
    }
    return false;
  }

  /**
   * Get the guard branch condition
   */
  Value *getGuardBranchCondition(BasicBlock *BB) {
    if (auto *branchInst = dyn_cast<BranchInst>(BB->getTerminator()))
      if (branchInst->isConditional())
        return branchInst->getCondition();
    return nullptr;
  }

  /**
   * Check if both guard condition are equal by comparing every component
   * of the condition
   */
  bool areGuardConditionEquivalent(BasicBlock *firGuard, BasicBlock *secGuard) {
    if (auto *firCond = dyn_cast<ICmpInst>(getGuardBranchCondition(firGuard))) {
      if (auto *secCond =
              dyn_cast<ICmpInst>(getGuardBranchCondition(secGuard))) {
        if (firCond->getPredicate() == secCond->getPredicate() &&
            firCond->getOperand(0) == secCond->getOperand(0) &&
            firCond->getOperand(1) == secCond->getOperand(1)) {
          utils::debug("Both guards have the same condition");
          return true;
        }
      }
    }
    return false;
  }

  /**
   * Runs adjacency analysis
   */
  bool adjacentAnalysis(Loop *FirstLoop, Loop *SecondLoop,
                        bool customGetter = false) {

    utils::debugYesNo("Which guard getter:  \t", customGetter, "Custom",
                      "Predefined");

    BasicBlock *firGuard = getLoopGuard(FirstLoop, customGetter),
               *secGuard = getLoopGuard(SecondLoop, customGetter);

    utils::debugYesNo("First Loop guarded?: \t", firGuard);
    utils::debugYesNo("Second Loop guarded?:\t", secGuard);

    // both are guarded
    if (firGuard != nullptr && secGuard != nullptr) {
      utils::debug("Both are guarded");
      return firstGuardPointsSecondGuard(firGuard, secGuard) &&
             areGuardConditionEquivalent(firGuard, secGuard);
    }

    // both are unguarded
    if (firGuard == nullptr && secGuard == nullptr) {
      utils::debug("Both are unguarded");
      return isFirstNextToSecond(FirstLoop, SecondLoop);
    }

    // one is guarded
    utils::debug("Only one is guarded");
    return false;
  }

  /**
   * Analyze two loops
   */
  bool analyzeCouple(Loop *FirstLoop, Loop *SecondLoop, DominatorTree *DT,
                     PostDominatorTree *PDT) {
    outs() << "\033[1;38:5:255m[LFP-ADJ] First loop:\033[0m "
           << "\033[0;38:5:255m" << FirstLoop->getLoopID()
           << ", Depth: " << FirstLoop->getLoopDepth() << "\033[0m\n"; // DEBUG
    outs() << "\033[1;38:5:255m[LFP-ADJ] Second loop:\033[0m "
           << "\033[0;38:5:255m" << SecondLoop->getLoopID()
           << ", Depth: " << SecondLoop->getLoopDepth() << "\033[0m\n"; // DEBUG

    if (!FirstLoop || !SecondLoop) {
      utils::debug("At least one of the two loops is null");
      return false;
    }

    if (!FirstLoop->isLoopSimplifyForm() || !SecondLoop->isLoopSimplifyForm()) {
      utils::debug("At least one of the two loops isn't in simplify form");
      return false;
    }

    bool isCoupleAdjacent = adjacentAnalysis(FirstLoop, SecondLoop);
    utils::debugYesNo("Is couple Adjacent?:\t", isCoupleAdjacent);

    bool isCoupleCFE = areControlFlowEquivalent(FirstLoop, SecondLoop, DT, PDT);
    utils::debugYesNo("Is couple CFE?: \t", isCoupleCFE);

    return isCoupleAdjacent && isCoupleCFE;
  }

  /**
   *
   */
  bool areControlFlowEquivalent(Loop *FirstLoop, Loop *SecondLoop,
                                DominatorTree *DT, PostDominatorTree *PDT) {
    return (DT->dominates(FirstLoop->getHeader(), SecondLoop->getHeader()) &&
            PDT->dominates(SecondLoop->getHeader(), FirstLoop->getHeader()));
  }

  bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
    outs() << "\033[1;38:5:40m[LF] Run on function:\033[0m "
           << "\033[0;38:5:40m" << F.getName() << "\033[0m\n"; // DEBUG

    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);

    const std::vector<Loop *> LoopVect = LI.getTopLevelLoops();

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
