//============================================================================//
// FILE:
//    PassRegister.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion.
//
// EXECUTE WITH PASSES:
//    -passes="loop(loop-rotate),function(loop-simplify),function(gb-custom-ndd-analyser)"
//
// License: GPL3
//============================================================================//
#include "Utils.hpp"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <llvm/Analysis/DependenceAnalysis.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
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
  bool analyzeCouple(Loop *FL, Loop *SL, DependenceInfo *DI) const {
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

    bool isCoupleNotNDD = !areNegativeDistanceDependent(FL, SL, DI);
    utils::debugYesNo("[LF-NDD] Couple isn't NDD: \t", isCoupleNotNDD);

    return isCoupleNotNDD;
  }

  /* ------------------------------------------------------------------------ */

  bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
    utils::debug("\n\033[1;38:5:15m "
                 ">============================================================"
                 "========<\033[0m");
    utils::debug("\n[LF] Run on function: " + F.getName().str(), utils::GREEN);

    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
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

        bool IsValid = analyzeCouple(First, Second, &DI);

        if (!IsValid)
          continue;

        utils::debug("[LF] Valid couple found.", utils::PURPLE);
      }
    }

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
                  if (Name == "gb-custom-ndd-analyser") {
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
