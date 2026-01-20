
//==============================================================================
// FILE:
//    LoopFusion.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#include "LoopFusion.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  /**
   * Entry point.
   */
  void LoopFusion::fuse() {
    fuseLoops();
  }

  void LoopFusion::constructCandidate(Loop *loop, LFCandidate *LFC) {
    LFC->loop = loop;
    LFC->indPhi = loop->getCanonicalInductionVariable();
    LFC->incInst = getLoopIncrementInstruction(loop);
    LFC->exit = loop->getExitBlock();
    LFC->header = loop->getHeader();
    LFC->headerBranch = dyn_cast<BranchInst>(LFC->header->getTerminator());
    LFC->latch = loop->getLoopLatch();
    LFC->guardBranch = loop->getLoopGuardBranch();
    LFC->preheader = loop->getLoopPreheader();
  }

  /**
   * Returns the instruction that increments the induction variable inside the
   * loop. If not found, it returns nullptr.
   */
  Instruction *LoopFusion::getLoopIncrementInstruction(const Loop *L) const {
    if (PHINode *phi = L->getCanonicalInductionVariable())
      for (auto &U : phi->incoming_values())
        if (Instruction *phiUser = dyn_cast<Instruction>(U))
          if (L->contains(phiUser))
            return phiUser;
    return nullptr;
  }

  /**
   * Replace a branch instruction with another one
   */
  void LoopFusion::moveBranch(Instruction *L2incInst, PHINode *L2Phi,
                  BranchInst *L1BranchInst, BranchInst *L2branchInst) {
    // Move the increment instruction for the phi
    L2incInst->moveAfter(L2Phi);

    // Replace the first loop branch with the second loop one
    L1BranchInst->replaceAllUsesWith(L2branchInst);
    L2branchInst->moveBefore(L1BranchInst);
    L1BranchInst->eraseFromParent();
  }

  /**
   * Move all the phis at the top of the basic block.
   * All of the phis need to be at the top of the basic block.
   */
  void LoopFusion::orderPHIs(BasicBlock *BB) {
    for (auto &inst : *BB)
      if (isa<PHINode>(inst))
        inst.moveBefore(BB->getFirstNonPHI());
  }

  /**
   * Update the incomig basic block for the phis that are moved inside the first
   * loop header
   */
  void LoopFusion::updatePHIs(SmallVector<PHINode *, 8> *phiToUpdate, LFCandidate &lfc1) {
    for (PHINode *phi : *phiToUpdate) {
      // For phi instructions inside the header
      if (lfc1.header == phi->getParent()) {

        // For every incoming value for the phi
        for (unsigned i = 0; i < phi->getNumIncomingValues(); ++i) {

          Value *incVal = phi->getIncomingValue(i);

          if (Instruction *inc = dyn_cast<Instruction>(incVal)) {
            // If the incoming value is an instruction set the incoming basic
            // block as the latch of the first loop
            phi->setIncomingBlock(i, lfc1.latch);
          } else if (isa<Constant>(incVal)) {
            // If the incoming value is a constant set the incoming basic block
            // as the preheader of the first loop
            phi->setIncomingBlock(i, lfc1.preheader);
          }
        }
      } else { /* For simplicity we ignore phis located outside the header */ }
    }
    phiToUpdate->clear();
  }

  /**
   * Fuse two loops together
   */
  void LoopFusion::doFusion(LFCandidate &lfc1, LFCandidate &lfc2, bool secondDBL,
                bool bothDifferent) {

    SmallVector<Instruction *, 8> instToMove;
    SmallVector<PHINode *, 8> phiToUpdate;

    // Find the instructions from the second loop header
    // that need to be moved to the first loop header.
    for (Instruction &I : *lfc2.header)
      if (&I != lfc2.indPhi && &I != lfc2.headerBranch && &I != lfc2.incInst)
        instToMove.push_back(&I);

    // Move the instructions from the second loop header
    // to the first loop header, and collect phis that
    // need to be updated.
    for (auto I : instToMove) {
      I->moveBefore(lfc1.incInst);
      I->replaceUsesOfWith(lfc2.indPhi, lfc1.indPhi);
      if (PHINode *phi = dyn_cast<PHINode>(I))
        phiToUpdate.push_back(phi);
    }

    // After moving phi instruction from the second loop header to the first
    // one we need to update the incoming blocks of the phi instructions.
    updatePHIs(&phiToUpdate, lfc1);

    // If the second loop has the body and latch separated
    if (secondDBL) {

      // If also the first loop has the body and latch separated
      if (bothDifferent) {

        // Get the parent for the increment instrucion
        BasicBlock *L1IncInstBB = lfc1.incInst->getParent();
        // Get the branch instruction for the basic block that contains
        // the increment instruction
        BranchInst *L1IncInstBBBranchInst =
            dyn_cast<BranchInst>(L1IncInstBB->getTerminator());

        moveBranch(lfc2.incInst, lfc2.indPhi, L1IncInstBBBranchInst,
                   lfc2.headerBranch);
      } else {
        moveBranch(lfc2.incInst, lfc2.indPhi, lfc1.headerBranch,
                   lfc2.headerBranch);
      }

      SmallVector<BasicBlock *, 8> BBToMove;
      // Save the basic block to move from the second loop to the
      // first one
      for (auto S : lfc2.headerBranch->successors()) {
        BBToMove.push_back(S);
      }

      for (auto BB : BBToMove) {
        for (auto succ : successors(BB)) {
          if (!std::count(BBToMove.begin(), BBToMove.end(), succ)) {
            for (auto &inst : *BB) {
              // For every instruction for every basic block of the second loop
              // and for every successors replace its successor with the
              // latch of the first loop
              inst.replaceUsesOfWith(BB->getSingleSuccessor(), lfc1.latch);
              // Collect the phi to be updated if there are any
              // if (isa<PHINode>(inst)) {
              //   phiToUpdate.push_back(dyn_cast<PHINode>(&inst));
              // }
            }
          }
        }
        // Register the second loop basic block to the first loop
        lfc1.loop->addBasicBlockToLoop(BB, LI);
        // Move the second loop basic block to the first loop
        BB->moveBefore(lfc1.latch);
      }

      // Create a dummy branch instruction for the second loop:
      // if you don't plan to erase unused basick block LLVM will throw
      // an exception because it will not find a terminator instruction
      // for the second loop
      BranchInst::Create(lfc2.latch, lfc2.header);
      lfc2.indPhi->replaceAllUsesWith(lfc1.indPhi);
    }

    // Updating Phi Instructions Incoming Blocks
    // updatePHIs(&phiToUpdate, lfc1);

    // Swap the exit of L1 with the exit of L2
    for (auto BB : predecessors(lfc1.exit)) {
      for (auto &I : *BB) {
        I.replaceUsesOfWith(lfc1.exit, lfc2.exit);
      }
    }

    // Swap the exit of the guard with the exit of L2
    if (lfc1.guardBranch) {
      for (auto succ : successors(lfc1.guardBranch)) {
        if (succ != lfc1.preheader) {
          lfc1.guardBranch->replaceUsesOfWith(succ, lfc2.exit);
        }
      }
    }

    // Reorder the phis
    orderPHIs(lfc1.header);

    // Eliminate unused basic blocks, usually l1exit, l2header and l2latch
    EliminateUnreachableBlocks(F);

  }

  /**
   * Check if the loop has the body and the latch combined
   */
  bool LoopFusion::combinedBodyAndLatch(Loop *L) {

    BasicBlock *latch = L->getLoopLatch();
    BasicBlock *body = L->getHeader()->getNextNode();
    BasicBlock *last =
        body == latch ? L->getExitBlock()->getPrevNode() : latch->getPrevNode();

    // If these three are the same it means there is only the latch basic block
    // between the loop header and the exit.
    return (last == body && body == latch);
  }

  void LoopFusion::fuseLoops() {

    LFCandidate lfc1, lfc2;
    // Populate lfc1 and lfc2
    constructCandidate(firstLoop, &lfc1);
    constructCandidate(secondLoop, &lfc2);

    // If second loop has combined body and latch.
    // The first loop could have combined body and latch or not , but we care
    // only to move the instructions of the second loop header inside the
    // first loop one
    if (combinedBodyAndLatch(secondLoop)) {
      utils::debug("[LF-LF] Second loop has the body and the latch combined.",
                   utils::BLUE);

      doFusion(lfc1, lfc2, false, false);
      return;
    }

    // If first loop has body and latch combined.
    // The second loop has not.
    if (combinedBodyAndLatch(firstLoop)) {
      utils::debug("[LF-LF] First loop has the body and the latch combined.",
        utils::BLUE);

      doFusion(lfc1, lfc2, true, false);
      return;
    }

    // If both loop have different body and latch
    if (!combinedBodyAndLatch(firstLoop) && !combinedBodyAndLatch(secondLoop)) {
      utils::debug("[LF-LF] Both loops have the body and the latch different.",
                   utils::BLUE);

      doFusion(lfc1, lfc2, true, true);
      return;
    }
  }

} // namespace graboidpasses::lf
