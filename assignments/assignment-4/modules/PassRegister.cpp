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
#include <llvm/ADT/DepthFirstIterator.h>
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

#include "Utils.hpp"

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

  bool areNegativeDistanceDependent(Loop *firstLoop, Loop *secondLoop,
                                    DependenceInfo *DI) const {

    std::vector<StoreInst *> FirstLoopStoreInst, SecondLoopStoreInst;
    std::vector<LoadInst *> FirstLoopLoadInst, SecondLoopLoadInst;

    getLoopInstructionByType(firstLoop, &FirstLoopStoreInst,
                             &FirstLoopLoadInst);
    getLoopInstructionByType(secondLoop, &SecondLoopStoreInst,
                             &SecondLoopLoadInst);

    for (StoreInst *Store : FirstLoopStoreInst)
      for (LoadInst *Load : SecondLoopLoadInst)
        if (DI->depends(Store, Load, true) && isDistanceNegative(Store, Load))
          return true;

    for (LoadInst *Load : FirstLoopLoadInst)
      for (StoreInst *Store : SecondLoopStoreInst)
        if (DI->depends(Store, Load, true) && isDistanceNegative(Store, Load))
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
    utils::debugYesNo("[LF-NDD] Couple isn't NDD: \t", isCoupleNotNDD);

    return isCoupleAdjacent && isCoupleCFE && isCoupleTCE && isCoupleNotNDD;
  }

  /* ------------------------------------------------------------------------ */

  struct {
    Loop *loop;
    PHINode *phi;
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
    LFC->phi = loop->getCanonicalInductionVariable();
    LFC->incInst = getLoopIncrementInstruction(loop);
    LFC->exit = loop->getExitBlock();
    LFC->header = loop->getHeader();
    LFC->headerBranch = dyn_cast<BranchInst>(LFC->header->getTerminator());
    LFC->latch = loop->getLoopLatch();
    LFC->guardBranch = loop->getLoopGuardBranch();
    LFC->preheader = loop->getLoopPreheader();
  }

  Instruction *getLoopIncrementInstruction(Loop *L) {
    PHINode *phi = L->getCanonicalInductionVariable();
    for (auto &U : phi->incoming_values()) {
      if (Instruction *phiUser = dyn_cast<Instruction>(U)) {
        if (L->contains(phiUser)) {
          return phiUser;
        }
      }
    }
    return nullptr;
  }

  void moveBranch(Instruction *L2incInst, PHINode *L2Phi,
                  BranchInst *L1BranchInst, BranchInst *L2branchInst) {
    L2incInst->moveAfter(L2Phi);
    L1BranchInst->replaceAllUsesWith(L2branchInst);

    L2branchInst->moveBefore(L1BranchInst);
    L1BranchInst->eraseFromParent();
  }

  void secondDifferentBodyAndLatch(LFCandidate &lfc1, LFCandidate &lfc2,
                                   bool bothDifferent) {

    if (bothDifferent) {

      BasicBlock *L1IncInstBB = lfc1.incInst->getParent();
      BranchInst *L1IncInstBBBranchInst =
          dyn_cast<BranchInst>(L1IncInstBB->getTerminator());

      moveBranch(lfc2.incInst, lfc2.phi, L1IncInstBBBranchInst,
                 lfc2.headerBranch);
    } else {
      moveBranch(lfc2.incInst, lfc2.phi, lfc1.headerBranch, lfc2.headerBranch);
    }

    SmallVector<BasicBlock *, 8> BBToMove;
    for (auto S : lfc2.headerBranch->successors()) {
      BBToMove.push_back(S);
    }

    for (auto BB : BBToMove) {

      for (auto succ : successors(BB)) {
        if (!std::count(BBToMove.begin(), BBToMove.end(), succ)) {
          for (auto &inst : *BB) {
            inst.replaceUsesOfWith(BB->getSingleSuccessor(), lfc1.latch);
          }
        }
      }
      BB->moveBefore(lfc1.latch);
    }

    BranchInst::Create(lfc2.latch, lfc2.header);
  }

  bool combinedBodyAndLatchFusion(LFCandidate &lfc1, LFCandidate &lfc2,
                                  bool secondDBL, bool bothDifferent) {

    // Find the instructions that need to be moved
    SmallVector<Instruction *, 8> toMove;
    for (auto I = lfc2.header->begin(); I != lfc2.header->end(); I++) {
      Instruction *II = dyn_cast<Instruction>(I);
      if (II != lfc2.phi && II != lfc2.headerBranch && II != lfc2.incInst) {
        toMove.push_back(II);
      }
    }

    // Move the instructions
    for (auto I : toMove) {
      I->moveBefore(lfc1.incInst);
      I->replaceUsesOfWith(lfc2.phi, lfc1.phi);
    }

    if (secondDBL)
      secondDifferentBodyAndLatch(lfc1, lfc2, bothDifferent);

    // Swap the exit of L1 with the exit of L2
    for (auto U : predecessors(lfc1.exit)) {
      for (auto &I : *U) {
        I.replaceUsesOfWith(lfc1.exit, lfc2.exit);
      }
    }

    // RECONSTRUCT THE CFG
    // DT.recalculate(F);
    // PDT.recalculate(F);

    if (lfc1.guardBranch) {
      for (auto succ : successors(lfc1.guardBranch)) {
        if (succ != lfc1.preheader) {
          lfc1.guardBranch->replaceUsesOfWith(succ, lfc2.exit);
        }
      }
    }

    // If necessary, remove the basic blocks that will never be executed
    // if (L2GuardBranch)
    //   L2Guard->eraseFromParent();
    // L1exit->eraseFromParent();
    // L2Header->eraseFromParent();
    // L2->getLoopLatch()->eraseFromParent();

    return true;
  }

  bool combinedBodyAndLatch(Loop *L) {

    BasicBlock *latch = L->getLoopLatch();
    BasicBlock *body = L->getHeader()->getNextNode();
    BasicBlock *last =
        body == latch ? L->getExitBlock()->getPrevNode() : latch->getPrevNode();

    return (last == body && body == latch);
  }

  bool fuseLoops(Loop *firstLoop, Loop *secondLoop) {

    LFCandidate lfc1, lfc2;
    constructCandidate(firstLoop, &lfc1);
    constructCandidate(secondLoop, &lfc2);

    if (combinedBodyAndLatch(secondLoop)) {
      utils::debug("[LF-LF] Second loop has the body and the latch combined.",
                   utils::BLUE);
      return combinedBodyAndLatchFusion(lfc1, lfc2, false, false);
    }

    if (combinedBodyAndLatch(firstLoop)) {
      utils::debug("[LF-LF] First loop has the body and the latch combined.",
                   utils::BLUE);
      return combinedBodyAndLatchFusion(lfc1, lfc2, true, false);
    }

    if (!combinedBodyAndLatch(firstLoop) && !combinedBodyAndLatch(secondLoop)) {
      utils::debug("[LF-LF] Both loops have the body and the latch different.",
                   utils::BLUE);
      return combinedBodyAndLatchFusion(lfc1, lfc2, true, true);
    }

    return false;
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

      if (isCoupleValid)
        utils::debug("[LF] Couple is valid.", utils::PURPLE);

      bool isCoupleFused =
          isCoupleValid && fuseLoops(Worklist[first], Worklist[second]);

      if (isCoupleFused) {
        utils::debug("[LF] Couple fused successfully.", utils::PURPLE);
        outs() << '\n';
        F.print(outs());
        outs() << '\n';
      }

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
