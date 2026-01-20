//============================================================================//
// FILE:
//    PassRegister.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion.
//
// EXECUTE WITH PASSES:
//    -passes="loop(loop-rotate),function(loop-simplify),function(gb-loop-fusion)"
//
// License: GPL3
//============================================================================//
#include "Utils.hpp"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <llvm/Analysis/DependenceAnalysis.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/IR/PassManager.h>

using namespace llvm;

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
   * Returns the guard of the loop if it exists.
   */
  BasicBlock *getLoopGuard(Loop *L) const {
    if (BranchInst *br = L->getLoopGuardBranch())
      return br->getParent();
    return nullptr;
  }

  /**
   * Checks for unwanted instructions inside the preheader.
   */
  bool isCleanFromUnwantedInstructions(BasicBlock *BB,
                                     std::set<Instruction *> whitelist) const {
    for (Instruction &I : *BB)
      if (whitelist.count(&I) != 1)
        return false;
    return true;
  }

  /**
   * Check if the first guard points to the second guard.
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
   * Get the guard branch condition.
   */
  Value *getGuardBranchCondition(BasicBlock *BB) const {
    if (auto *branchInst = dyn_cast<BranchInst>(BB->getTerminator()))
      if (branchInst->isConditional())
        return branchInst->getCondition();
    return nullptr;
  }

  /**
   * Check if both guard conditions are equal by comparing every component
   * of the condition.
   */
  bool areGuardsEquivalent(BasicBlock *firGuard, BasicBlock *secGuard) const {
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

  /* ------------------------------------------------------------------------ */

  /**
   * Checks if the first loop is adjacent to the second one and if they are
   * clean so that they don't contain unwanted instructions.
   */
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

  /**
   * Checks if the first loop is adjacent to the second one and if they are
   * clean so that they don't contain unwanted instructions.
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
    if (!isCleanFromUnwantedInstructions(SL_preheader,
                                       {SL_preheader->getTerminator()})) {
      utils::debug("[LF-ADJ] Preheader not clean.", utils::YELLOW);
      return false;
    }

    return true;
  }

  /**
   * Runs adjacency analysis, checking first if both loops are guarded or
   * unguarded. In case only one of them is guarded, no adjacency check is
   * performed.
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

  /**
   * Checks if the two loops iterate the same number of times.
   * @see <a
   * href="https://llvm.org/devmtg/2018-04/slides/Absar-ScalarEvolution.pdf">
   * Scalar Evolution </a>
   */
  bool areTripCountEquivalent(Loop *firstLoop, Loop *secondLoop,
                              ScalarEvolution *SE) const {

    if (!SE->hasLoopInvariantBackedgeTakenCount(firstLoop) ||
        !SE->hasLoopInvariantBackedgeTakenCount(secondLoop)) {
      utils::debug("[LF-TCE] At least one of the loops hasn't got a loop "
                   "invariant backedge.",
                   utils::YELLOW);
      return false;
    }

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

  /**
   * Recursively evaluates a Value representing an index expression.
   * Supports constants, casts, binary operations and a simplified PHI handling.
   * Uses a conservative cycle detection limited to PHI nodes.
   */
  int64_t resolveIndexValue(Value *curr,
                            std::unordered_set<Value *> &visitedPHIs,
                            bool &failed, bool failPHIs = true) const {
    if (failed || !curr)
      return 0;

    // Constant
    if (auto *CI = dyn_cast<ConstantInt>(curr))
      return CI->getSExtValue();

    auto *I = dyn_cast<Instruction>(curr);
    if (!I) {
      utils::debug("[LF-NDD] Unsupported value type.", utils::YELLOW);
      failed = true;
      return 0;
    }

    // Casts
    if (auto *Cast = dyn_cast<CastInst>(I)) {
      if (isa<SExtInst>(Cast) || isa<ZExtInst>(Cast))
        return resolveIndexValue(Cast->getOperand(0), visitedPHIs, failed, failPHIs);
    }

    // PHI
    if (auto *PHI = dyn_cast<PHINode>(I)) {

      /* Conservative Failure */
      if (failPHIs) {
        utils::debug("[LF-NDD] PHI node encountered, conservative failure.",
                    utils::YELLOW);
        failed = true;
        return 0;
      }

      /* Cycle Detectection */
      if (visitedPHIs.count(curr)) {
        return 0;
      }

      visitedPHIs.insert(curr);

      int64_t init = resolveIndexValue(
        PHI->getIncomingValue(0), visitedPHIs, failed, failPHIs);
      int64_t step = resolveIndexValue(
        PHI->getIncomingValue(1), visitedPHIs, failed, failPHIs);

      return init + step;

      /* This approximation assumes a canonical loop PHI with exactly two incoming
       * values, modeling a simple induction variable.
       * - 'init' represents the initial value before entering the loop.
       * - 'step' represents a constant increment applied at each iteration.
       *
       * The analysis assumes that 'step' is loop-invariant and does not change
       * across iterations. It also ignores the long-term evolution of the index:
       * even if the load index grows faster than the store index, only the
       * first-iteration ordering is considered.
       */
    }

    // Binary operations
    if (!I->isBinaryOp()) {
      utils::debug("[LF-NDD] Unsupported instruction type.", utils::YELLOW);
      failed = true;
      return 0;
    }

    int64_t lhs = resolveIndexValue(I->getOperand(0), visitedPHIs, failed, failPHIs);
    int64_t rhs = resolveIndexValue(I->getOperand(1), visitedPHIs, failed, failPHIs);

    if (failed)
      return 0;

    switch (I->getOpcode()) {
    case Instruction::Add:
      return lhs + rhs;
    case Instruction::Sub:
      return lhs - rhs;
    case Instruction::Mul:
      return lhs * rhs;
    case Instruction::SDiv:
    case Instruction::UDiv:
      if (rhs == 0) {
        utils::debug("[LF-NDD] Division by zero.", utils::YELLOW);
        failed = true;
        return 0;
      }
      return lhs / rhs;
    default:
      utils::debug("[LF-NDD] Unsupported binary opcode.", utils::YELLOW);
      failed = true;
      return 0;
    }
  }

  /**
   * Entry point for index extraction starting from an instruction.
   * Initializes PHI cycle tracking and delegates evaluation to resolveIndexValue.
   */
  int64_t extractIndex(Instruction *I, bool &failed) const {
    if (failed || !I)
      return 0;

    bool failPHIs = false; // Fail evaluation upon finding a PHI instruction
    std::unordered_set<Value *> visitedPHIs;
    return resolveIndexValue(I, visitedPHIs, failed, failPHIs);
  }

  /**
   * Evaluates a flattened index expression used inside GEP instructions.
   * Handles constant factors, casted indices and multiplicative expressions.
   * Designed for simple affine-like patterns only.
   */
  int64_t evaluateIndexExpr(Value *V, bool &failed) const {
    if (failed || !V)
      return 0;

    // Constant
    if (auto *CI = dyn_cast<ConstantInt>(V)) {
      return CI->getSExtValue();
    }

    // SExt / ZExt
    if (auto *Ext = dyn_cast<CastInst>(V)) {
      if (isa<SExtInst>(Ext) || isa<ZExtInst>(Ext)) {
        return extractIndex(Ext, failed);
      }
    }

    // Instruction
    if (auto *I = dyn_cast<Instruction>(V)) {

      // Mul
      if (I->getOpcode() == Instruction::Mul) {
        int64_t result = 1;

        for (Value *Op : I->operands()) {
          result *= evaluateIndexExpr(Op, failed);
          if (failed) return 0;
        }
        return result;
      }
    }

    // Unsupported case
    failed = true;
    return 0;
  }

  /**
   * Computes the index contribution of a single GEP instruction.
   * Each index operand is evaluated and summed to form the level index.
   */
  int64_t computeGEPLevelOffset(GetElementPtrInst *gep, bool &failed) const {
    if (failed || !gep)
      return 0;

    int64_t offset = 0;

    for (Value *Idx : gep->indices()) {
      offset += evaluateIndexExpr(Idx, failed);
      if (failed) return 0;
    }

    return offset;
  }

  /**
   * Iteratively traverses a chain of nested GEP instructions.
   * Accumulates indexes until the base Alloca instruction is reached.
   */
  int64_t computeTotalGEPOffset(GetElementPtrInst *gep, Value *&alloca,
                                bool &failed) const {
    if (!gep) {
      failed = true;
      return 0;
    }

    unsigned depth = 0;
    int64_t totalOffset = 0;
    Value *currentPtr = gep;

    while (auto *currentGEP = dyn_cast<GetElementPtrInst>(currentPtr)) {
      totalOffset += computeGEPLevelOffset(currentGEP, failed);
      if (failed) return 0;

      currentPtr = currentGEP->getPointerOperand();
      depth++;
    }

    if (isa<AllocaInst>(currentPtr)) {
      alloca = currentPtr;
      utils::debug("[LF-NDD] Array Depth: " + std::to_string(depth));
    } else {
      failed = true;
    }

    return totalOffset;
  }

  /**
   * Computes and compares memory indexes of a store-load pair.
   * Returns true if a negative dependency distance is conservatively detected.
   */
  bool isDistanceNegativeCustom(StoreInst *store, LoadInst *load) const {
    GetElementPtrInst *storeGEP = dyn_cast<GetElementPtrInst>(
                          store->getPointerOperand()),
                      *loadGEP = dyn_cast<GetElementPtrInst>(
                          load->getPointerOperand());

    if (!storeGEP || !loadGEP) {
      utils::debug("[LF-NDD] One of the pointer operands is not a GEP.",
                   utils::YELLOW);
      return false;
    }

    bool failed = false;
    Value *storeAlloca = nullptr;
    Value *loadAlloca = nullptr;

    int64_t storeResult = computeTotalGEPOffset(storeGEP, storeAlloca, failed);
    int64_t loadResult = computeTotalGEPOffset(loadGEP, loadAlloca, failed);

    if (storeAlloca != loadAlloca) {
      utils::debug(
          "[LF-NDD] The load and store don't refer to the same allocation.",
          utils::YELLOW);
      return false;
    }

    if (failed) {
      utils::debug("[LF-NDD] Operation failed! Assuming negative dependant.",
                   utils::YELLOW);
      return true;
    }

    utils::debug("[LF-NDD] New Couple:");
    utils::debug("[LF-NDD] --> Store Result: " + std::to_string(storeResult));
    utils::debug("[LF-NDD] --> Load Result: " + std::to_string(loadResult));

    if (storeResult < loadResult) {
      utils::debug("[LF-NDD] The difference is negative!", utils::YELLOW);
      return true;
    }

    return false;
  }

  /* ------------------------------------------------------------------------ */

  /**
   * Iterates through the instructions in the loop, saving only the load/store
   * instructions.
   */
  void getLoopInstructionByType(Loop *L, std::vector<StoreInst *> *stores,
                                std::vector<LoadInst *> *loads) const {
    for (BasicBlock *BB : L->getBlocks()) {
      for (Instruction &I : *BB) {

        if (StoreInst *store = dyn_cast<StoreInst>(&I))
          stores->push_back(store);

        else if (LoadInst *load = dyn_cast<LoadInst>(&I))
          loads->push_back(load);
      }
    }
  }

  /**
   * Checks if the load and store instructions of the two loops are dependent.
   * If they are, then checks if their dependency distance is negative.
   */
  bool areNegativeDistanceDependent(Loop *firstLoop, Loop *secondLoop,
                                    DependenceInfo *DI) const {

    std::vector<StoreInst *> FirstLoopStoreInst, SecondLoopStoreInst;
    std::vector<LoadInst *> FirstLoopLoadInst, SecondLoopLoadInst;

    getLoopInstructionByType(firstLoop, &FirstLoopStoreInst,
                             &FirstLoopLoadInst);
    getLoopInstructionByType(secondLoop, &SecondLoopStoreInst,
                             &SecondLoopLoadInst);

    // Checking the dependency between the store instruction of the first loop
    // and the load instruction of the second loop
    for (StoreInst *Store : FirstLoopStoreInst)
      for (LoadInst *Load : SecondLoopLoadInst)
        if (DI->depends(Store, Load, true) && isDistanceNegativeCustom(Store, Load))
          return true;

    // Checking the dependency between the load instruction of the first loop
    // and the store instruction of the second loop
    for (LoadInst *Load : FirstLoopLoadInst)
      for (StoreInst *Store : SecondLoopStoreInst)
        if (DI->depends(Store, Load, true) && isDistanceNegativeCustom(Store, Load))
          return true;

    return false;
  }

  /* ------------------------------------------------------------------------ */

  /**
   * Analyzes two loops and check if they are adjacent, control flow equivalent,
   * trip count equivalent and if there are any negative distance dependencies
   * between them. Both loops must be in simplify and rotated form.
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
    utils::debugYesNo("[LF-NDD] Couple isn't NDD: \t", isCoupleNotNDD);

    return isCoupleAdjacent && isCoupleCFE && isCoupleTCE && isCoupleNotNDD;
  }

  /* ------------------------------------------------------------------------ */

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

  void constructCandidate(Loop *loop, LFCandidate *LFC) {
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
  Instruction *getLoopIncrementInstruction(const Loop *L) const {
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
  void moveBranch(Instruction *L2incInst, PHINode *L2Phi,
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
  void orderPHIs(BasicBlock *BB) {
    for (auto &inst : *BB)
      if (isa<PHINode>(inst))
        inst.moveBefore(BB->getFirstNonPHI());
  }

  /**
   * Update the incomig basic block for the phis that are moved inside the first
   * loop header
   */
  void updatePHIs(SmallVector<PHINode *, 8> *phiToUpdate, LFCandidate &lfc1) {
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
  void doFusion(LFCandidate &lfc1, LFCandidate &lfc2, bool secondDBL,
                bool bothDifferent, LoopInfo &LI, Function &F) {

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
  bool combinedBodyAndLatch(Loop *L) {

    BasicBlock *latch = L->getLoopLatch();
    BasicBlock *body = L->getHeader()->getNextNode();
    BasicBlock *last =
        body == latch ? L->getExitBlock()->getPrevNode() : latch->getPrevNode();

    // If these three are the same it means there is only the latch basic block
    // between the loop header and the exit.
    return (last == body && body == latch);
  }

  void fuseLoops(Loop *firstLoop, Loop *secondLoop, LoopInfo &LI, Function &F) {

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

      doFusion(lfc1, lfc2, false, false, LI, F);
      return;
    }

    // If first loop has body and latch combined.
    // The second loop has not.
    if (combinedBodyAndLatch(firstLoop)) {
      utils::debug("[LF-LF] First loop has the body and the latch combined.",
        utils::BLUE);

      doFusion(lfc1, lfc2, true, false, LI, F);
      return;
    }

    // If both loop have different body and latch
    if (!combinedBodyAndLatch(firstLoop) && !combinedBodyAndLatch(secondLoop)) {
      utils::debug("[LF-LF] Both loops have the body and the latch different.",
                   utils::BLUE);

      doFusion(lfc1, lfc2, true, true, LI, F);
      return;
    }
  }

  /* ------------------------------------------------------------------------ */

  bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
    utils::debug("\n\033[1;38:5:15m "
                 ">============================================================"
                 "========<\033[0m");
    utils::debug("\n[LF] Run on function: " + F.getName().str(), utils::GREEN);

    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
    ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
    DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

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

    bool Transformed = false;

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

        bool IsValid = analyzeCouple(
          First, Second, &DT, &PDT, &SE, &DI
        );

        if (!IsValid)
          continue;

        utils::debug("[LF] Valid couple found.", utils::PURPLE);

        fuseLoops(First, Second, LI, F);

        utils::debug("[LF] Couple fused successfully.", utils::PURPLE);

        outs() << '\n';
        F.print(outs());
        outs() << '\n';

        Transformed = true;
      }
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
