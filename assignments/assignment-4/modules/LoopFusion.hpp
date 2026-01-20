//==============================================================================
// FILE:
//    LoopFusion.hpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#pragma once
#ifndef GRABOID_LF_LOOP_FUSION_H
#define GRABOID_LF_LOOP_FUSION_H
#include <llvm/Analysis/LoopInfo.h>
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "Utils.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  struct {
    Loop *loop;
    PHINode *indPhi;
    Instruction *incInst;
    BasicBlock *exit;
    BasicBlock *header;
    BranchInst *headerBranch;
    BasicBlock *latch;
    BranchInst *guardBranch;
    BasicBlock *preheader;
  } typedef LFCandidate;

  class LoopFusion {
  public:
    LoopFusion(Loop *L1, Loop *L2, LoopInfo &LI, Function &F)
        : firstLoop(L1), secondLoop(L2), LI(LI), F(F) {}

    void fuse();

  private:
    Loop *firstLoop;
    Loop *secondLoop;
    LoopInfo &LI;
    Function &F;

    void constructCandidate(Loop *loop, LFCandidate *LFC);
    Instruction *getLoopIncrementInstruction(const Loop *L) const;
    void moveBranch(Instruction *L2incInst, PHINode *L2Phi,
                    BranchInst *L1BranchInst, BranchInst *L2branchInst);
    void orderPHIs(BasicBlock *BB);
    void updatePHIs(SmallVector<PHINode *, 8> *phiToUpdate, LFCandidate &lfc1);
    void doFusion(LFCandidate &lfc1, LFCandidate &lfc2, 
                  bool secondDBL, bool bothDifferent);
    bool combinedBodyAndLatch(Loop *L);
    void fuseLoops();
  };

} // namespace graboidpasses::lf

#endif  // GRABOID_LF_LOOP_FUSION_H
