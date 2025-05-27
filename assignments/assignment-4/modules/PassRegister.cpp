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
#include <llvm/ADT/DepthFirstIterator.h>
#include <llvm/ADT/SmallBitVector.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/DependenceAnalysis.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Operator.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Value.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/Error.h>
#include <vector>

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
  bool cleanFromUnwantedInstructions(BasicBlock *BB,
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
    if (!cleanFromUnwantedInstructions(SL_preheader,
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
   * Recursively resolves an integer value from a given value.
   */
  int64_t resolveIndexValue(Value *curr, std::set<Value *> *visited,
                            bool &failed) const {
    if (failed)
      return 0;

    if (Instruction *currInst = dyn_cast<Instruction>(curr)) {

      if (visited->count(curr) > 0)
        return 0;
      visited->insert(curr);

      // Handle SExt/ZExt early
      if (SExtInst *sext = dyn_cast<SExtInst>(currInst))
        return resolveIndexValue(sext->getOperand(0), visited, failed);
      if (ZExtInst *zext = dyn_cast<ZExtInst>(currInst))
        return resolveIndexValue(zext->getOperand(0), visited, failed);

      if (PHINode *phi = dyn_cast<PHINode>(currInst)) {
        int64_t sum = 0;
        for (unsigned i = 0; i < phi->getNumIncomingValues(); ++i) {
          sum += resolveIndexValue(phi->getIncomingValue(i), visited, failed);
        }
        return sum;
      }

      if (!currInst->isBinaryOp()) {
        utils::debug("[LF-NDD] The instruction isn't binary.", utils::YELLOW);
        failed = true;
        return 0;
      }

      Value *op1 = currInst->getOperand(0);
      Value *op2 = currInst->getOperand(1);

      int64_t op1res = resolveIndexValue(op1, visited, failed);
      int64_t op2res = resolveIndexValue(op2, visited, failed);

      switch (currInst->getOpcode()) {
      case Instruction::Add:
        return op1res + op2res;
      case Instruction::Sub:
        return op1res - op2res;
      case Instruction::Mul:
        return op1res * op2res;
      case Instruction::UDiv:
      case Instruction::SDiv:
        if (op2res == 0) {
          utils::debug("[LF-NDD] Division by zero.", utils::YELLOW);
          failed = true;
          return 0;
        }
        return op1res / op2res;
      default:
        utils::debug("[LF-NDD] Unsupported operation type.", utils::YELLOW);
        failed = true;
        return 0;
      }
    } else if (ConstantInt *CI = dyn_cast<ConstantInt>(curr))
      return CI->getSExtValue();

    utils::debug("[LF-NDD] Unsupported instruction type.", utils::YELLOW);
    failed = true;
    return 0;
  }

  /**
   * Extracts and sums the values of the operands of the given SExt/ZExt
   * instruction.
   */
  int64_t extractIndex(Instruction *inst, bool &failed) const {
    if (failed)
      return 0;

    int64_t result = 0;
    std::set<Value *> visited = {};
    for (auto &oper : inst->operands()) {
      result += resolveIndexValue(oper, &visited, failed);
    }
    return result;
  }

  // i.e: ... (c * D2 * D3 * D4) + (i * D2 * D3) + (j * D3) + k

  /**
   * Recursively computes the product of operands of a given Mul instruction,
   * extracting values from constants and SExt/ZExt instructions.
   */
  int64_t computeOperandProduct(Instruction *current, bool &failed) const {
    if (failed)
      return 0;

    int64_t value = 1;

    // current is Mul
    if (current->getOpcode() == Instruction::Mul) {
      for (auto &oper : current->operands()) {
        Value *val = oper.get();
        if (Instruction *next = dyn_cast<Instruction>(val)) {

          // next is Mul
          if (next->getOpcode() == Instruction::Mul) {
            value *= computeOperandProduct(next, failed);
          }

          // next is SExt
          else if (isa<SExtInst>(next)) {
            // getting the current dimension "index"
            value *= extractIndex(next, failed);
          }

          // next is ZExt
          else if (isa<ZExtInst>(next)) {
            // getting the current dimension "total size"
            value *= extractIndex(next, failed);
          }

          // next is ConstantInt
        } else if (ConstantInt *CI = dyn_cast<ConstantInt>(val)) {
          return CI->getSExtValue();
        }
      }
    }

    return value;
  }

  /**
   * Computes the offset contribution of a single GEP instruction by analyzing
   * its indices.
   */
  int64_t computeGEPLevelOffset(GetElementPtrInst *gep, bool &failed) const {
    if (failed)
      return 0;

    int64_t offset = 0;

    for (auto &idxIt : gep->indices()) {
      Value *idx = idxIt;

      // if Constant
      if (ConstantInt *CI = dyn_cast<ConstantInt>(idx)) {
        offset += CI->getSExtValue();
      }

      // if SExt
      else if (SExtInst *idxSext = dyn_cast<SExtInst>(idx)) {
        offset += extractIndex(idxSext, failed);
      }

      // if Mul
      else if (Instruction *idxMul = dyn_cast<Instruction>(idx)) {
        if (idxMul->getOpcode() == Instruction::Mul) {
          offset += computeOperandProduct(idxMul, failed);
        }
      }

      // break; // we stop to the first found index.
    }
    return offset;
  }

  /**
   * Recursively computes the total offset from a nested GEP chain, identifying
   * the base alloca instruction and the array depth.
   */
  int64_t computeFullOffsetRecursive(GetElementPtrInst *gep, unsigned level,
                                     bool &failed, Value *&alloca) const {

    int64_t totalOffset = computeGEPLevelOffset(gep, failed);

    Value *ptr = gep->getPointerOperand();
    if (auto *nestedGEP = dyn_cast<GetElementPtrInst>(ptr)) {
      totalOffset +=
          computeFullOffsetRecursive(nestedGEP, level + 1, failed, alloca);
    } else if (isa<AllocaInst>(ptr)) {
      alloca = ptr;
      utils::debug("[LF-NDD] Array Depth: " + std::to_string(level + 1));
    }

    return totalOffset;
  }

  /**
   * Calculates the dependency distance between store and load instructions and
   * checks if it is negative.
   */
  bool isDistanceNegative(StoreInst *store, LoadInst *load) const {
    GetElementPtrInst *storeGEP = dyn_cast<GetElementPtrInst>(
                          store->getPointerOperand()),
                      *loadGEP = dyn_cast<GetElementPtrInst>(
                          load->getPointerOperand());

    if (!storeGEP || !loadGEP) {
      utils::debug("[LF-NDD] -- One of the pointer operands is not a GEP.",
                   utils::YELLOW);
      return false;
    }

    bool failed = false;
    Value *storeAlloca = nullptr, *loadAlloca = nullptr;

    // Calculating the offset on which the load/store instruction is performed
    int64_t storeResult =
        computeFullOffsetRecursive(storeGEP, 0, failed, storeAlloca);
    int64_t loadResult =
        computeFullOffsetRecursive(loadGEP, 0, failed, loadAlloca);

    if (storeAlloca != loadAlloca) {
      utils::debug(
          "[LF-NDD] -- The load and store don't refer to the same allocation.",
          utils::YELLOW);
      return false;
    }

    if (failed) {
      utils::debug("[LF-NDD] -- Operation failed! Assuming negative dependant.",
                   utils::YELLOW);
      return true;
    }

    utils::debug("[LF-NDD] New Couple:");
    utils::debug("[LF-NDD] -- Store Result: " + std::to_string(storeResult));
    utils::debug("[LF-NDD] -- Load Result: " + std::to_string(loadResult));

    if ((storeResult - loadResult) < 0) {
      utils::debug("[LF-NDD] -- The difference is negative!", utils::YELLOW);
      return true;
    }

    return false;
  }

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
        if (DI->depends(Store, Load, true) && isDistanceNegative(Store, Load))
          return true;

    // Checking the dependency between the load instruction of the first loop
    // and the store instruction of the second loop
    for (LoadInst *Load : FirstLoopLoadInst)
      for (StoreInst *Store : SecondLoopStoreInst)
        if (DI->depends(Store, Load, true) && isDistanceNegative(Store, Load))
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
    }

    // If first loop has body and latch combined.
    // The second loop has not.
    if (combinedBodyAndLatch(firstLoop)) {
      utils::debug("[LF-LF] First loop has the body and the latch combined.",
                   utils::BLUE);
      doFusion(lfc1, lfc2, true, false, LI, F);
    }

    // If both loop have different body and latch
    if (!combinedBodyAndLatch(firstLoop) && !combinedBodyAndLatch(secondLoop)) {
      utils::debug("[LF-LF] Both loops have the body and the latch different.",
                   utils::BLUE);
      doFusion(lfc1, lfc2, true, true, LI, F);
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

      unsigned long first = i, second = i - 1;

      bool isCoupleValid =
          analyzeCouple(Worklist[first], Worklist[second], &DT, &PDT, &SE, &DI);

      if (isCoupleValid) {
        utils::debug("[LF] Couple is valid.", utils::PURPLE);
        fuseLoops(Worklist[first], Worklist[second], LI, F);
        utils::debug("[LF] Couple fused successfully.", utils::PURPLE);

        outs() << '\n';
        F.print(outs());
        outs() << '\n';
      }

      Transformed |= isCoupleValid;
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
