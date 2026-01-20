
//==============================================================================
// FILE:
//    AdjacentAnalysis.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#include "AdjacentAnalysis.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  /**
   * Entry point.
   */
  bool AdjacentAnalysis::analyse() const {
    return processLoops();
  }

  /**
   * Runs adjacency analysis, checking first if both loops are guarded or
   * unguarded. In case only one of them is guarded, no adjacency check is
   * performed.
   */
  bool AdjacentAnalysis::processLoops() const {

    BasicBlock *firstGuard = getLoopGuard(firstLoop),
               *secondGuard = getLoopGuard(secondLoop);

    utils::debugYesNo("[LF-ADJ] First Loop guarded?: \t", firstGuard);
    utils::debugYesNo("[LF-ADJ] Second Loop guarded?:\t", secondGuard);

    // both are unguarded
    if (firstGuard == nullptr && secondGuard == nullptr) {
      utils::debug("[LF-ADJ] Both are unguarded.", utils::BLUE);
      return checkUnguardedLoopsAdjacency();
    }

    // both are guarded
    if (firstGuard != nullptr && secondGuard != nullptr) {
      utils::debug("[LF-ADJ] Both are guarded.", utils::BLUE);
      return checkGuardedLoopsAdjacency(firstGuard, secondGuard);
    }

    // one is guarded
    utils::debug("[LF-ADJ] Only one is guarded.", utils::YELLOW);
    return false;
  }

  /**
   * Checks if the first loop is adjacent to the second one and if they are
   * clean so that they don't contain unwanted instructions.
   */
  bool AdjacentAnalysis::checkUnguardedLoopsAdjacency() const {

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
    if (!isCleanFromUnwantedInstructions(SL_preheader,
                                       {SL_preheader->getTerminator()})) {
      utils::debug("[LF-ADJ] Preheader not clean.", utils::YELLOW);
      return false;
    }

    return true;
  }

  /**
   * Checks if the first loop is adjacent to the second one and if they are
   * clean so that they don't contain unwanted instructions.
   */
  bool AdjacentAnalysis::checkGuardedLoopsAdjacency(
    BasicBlock *firstGuard, BasicBlock *secondGuard) const {

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
    if (!isCleanFromUnwantedInstructions(FL_exitBlock,
                                       {FL_exitBlock->getTerminator()})) {
      utils::debug("[LF-ADJ] Exit block not clean.", utils::YELLOW);
      return false;
    }

    // Checking for unwanted instructions in the second loop preheader
    if (!isCleanFromUnwantedInstructions(SL_preheader,
                                       {SL_preheader->getTerminator()})) {
      utils::debug("[LF-ADJ] Preheader not clean.", utils::YELLOW);
      return false;
    }

    // Checking for unwanted instructions in the second loop guard
    if (!isCleanFromUnwantedInstructions(
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

    utils::debug("[LF-ADJ] Both guards have the same condition.", utils::BLUE);

    return true;
  }

  /* ------------------------------------------------------------------------ */

  /**
   * Returns the guard of the loop if it exists.
   */
  BasicBlock *AdjacentAnalysis::getLoopGuard(Loop *L) const {
    if (BranchInst *br = L->getLoopGuardBranch())
      return br->getParent();
    return nullptr;
  }

  /**
   * Checks for unwanted instructions inside the preheader.
   */
  bool AdjacentAnalysis::isCleanFromUnwantedInstructions(BasicBlock *BB,
                                     std::set<Instruction *> whitelist) const {
    for (Instruction &I : *BB)
      if (whitelist.count(&I) != 1)
        return false;
    return true;
  }

  /**
   * Check if the first guard points to the second guard.
   */
  bool AdjacentAnalysis::firstGuardPointsSecondGuard(BasicBlock *firGuard,
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
   * Get the guard branch condition.
   */
  Value *AdjacentAnalysis::getGuardBranchCondition(BasicBlock *BB) const {
    if (auto *branchInst = dyn_cast<BranchInst>(BB->getTerminator()))
      if (branchInst->isConditional())
        return branchInst->getCondition();
    return nullptr;
  }

  /**
   * Check if both guard conditions are equal by comparing every component
   * of the condition.
   */
  bool AdjacentAnalysis::areGuardsEquivalent(BasicBlock *firGuard, BasicBlock *secGuard) const {
    auto *firCond = dyn_cast<ICmpInst>(getGuardBranchCondition(firGuard));
    auto *secCond = dyn_cast<ICmpInst>(getGuardBranchCondition(secGuard));
    
    if (!firCond || !secCond)
      return false;
    
    return (
      firCond->getPredicate() == secCond->getPredicate() &&
      firCond->getOperand(0) == secCond->getOperand(0) &&
      firCond->getOperand(1) == secCond->getOperand(1) &&
      firCond->getType() == secCond->getType()
    );
  }

} // namespace graboidpasses::lf
