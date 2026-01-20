
//==============================================================================
// FILE:
//    NegativeDistanceDependence.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion pass.
//
// License: GPL3
//==============================================================================
#include "NegativeDistanceDependence.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  /**
   * Entry point.
   */
  bool NegativeDistanceDependenceAnalysis::analyse() const {
    return areNegativeDistanceDependent();
  }

  /**
   * Iterates through the instructions in the loop, saving only the load/store
   * instructions.
   */
  void NegativeDistanceDependenceAnalysis::getLoopInstructionByType(
    Loop *L, std::vector<StoreInst *> *stores, 
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
  bool NegativeDistanceDependenceAnalysis::areNegativeDistanceDependent() const {

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
        if (DI.depends(Store, Load, true) && isDistanceNegativeSE(
          firstLoop, secondLoop, Store, Load))
          return true;

    // Checking the dependency between the load instruction of the first loop
    // and the store instruction of the second loop
    for (LoadInst *Load : FirstLoopLoadInst)
      for (StoreInst *Store : SecondLoopStoreInst)
        if (DI.depends(Store, Load, true) && isDistanceNegativeSE(
          secondLoop, firstLoop, Store, Load))
          return true;

    return false;
  }

  /* ------------------------------------------------------------------------ */

  /**
   * Returns a simplified SCEVAddRecExpr of the pointer operand of a load/store.
   * Mdelling it into a polynomial recurrence of the form {base,+,stride}<loop>.
   * 
   * Example: a[i] inside a loop with stride 4 is {%a, +, 4}<for.body>
   */
  const SCEVAddRecExpr* NegativeDistanceDependenceAnalysis::getSCEVAddRecExpr(
    Loop *L, Instruction *I) const {
      // Get the pointer operand of the load/store
      Value *ptr = getLoadStorePointerOperand(I);

      // Get the SCEV at the scope of the loop
      const SCEV *ptrSCEV = SE.getSCEVAtScope(ptr, L);

      // Convert to an AddRecExpr (polynomial recurrence) if possible
      if (const SCEVAddRecExpr *rec = dyn_cast<SCEVAddRecExpr>(ptrSCEV)) {
          return rec;
      }

      return nullptr;
  }

  /**
   * Checks if the distance between two memory accesses is negative.
   * Only works for accesses with the same base and equal stride.
   *
   * Example:
   * store: a[i]       -> {%a, +, 4}<for.body>
   * load:  a[i+8]     -> {%a, +, 4}<for.body>
   * delta = base_store - base_load = 0 - 8 = -8 -> negative distance
   */
  bool NegativeDistanceDependenceAnalysis::isDistanceNegativeSE(
    Loop *storeLoop, Loop *loadLoop,
    StoreInst *store, LoadInst *load) const {

    // Get polynomial recurrences for both instructions
    const SCEVAddRecExpr *recS = getSCEVAddRecExpr(storeLoop, store);
    const SCEVAddRecExpr *recL = getSCEVAddRecExpr(loadLoop, load);

    // Cannot analyze if either recurrence is missing, assume dependant
    if (!recS || !recL) {
      
      utils::debug("[LF-NDD] Recurrence expression missing!"
        "Assuming negative dependant.", utils::YELLOW);

      return true;
    }

    // Check if the memory accesses have the same base pointer (alloca)
    // Example: a[i] vs b[i], different base pointer (alloca), no dependency
    if (SE.getPointerBase(recS) != SE.getPointerBase(recL)) return false;

    // Debug Output
    utils::debug("[LF-NDD] New L/S Couple Found:", utils::YELLOW);
    utils::debugValue("[LF-NDD] --> Store:", store, utils::YELLOW, "", '\t');
    utils::debugSCEV("", recS);
    utils::debugValue("[LF-NDD] --> Load: ", load, utils::YELLOW, "", '\t');
    utils::debugSCEV("", recL);

    // Extract the base (start of the recurrence) and stride
    const SCEV *baseS = recS->getStart(); // es, %a
    const SCEV *baseL = recL->getStart(); // es, %a

    const SCEV *strideS = recS->getStepRecurrence(SE); // es, 4
    const SCEV *strideL = recL->getStepRecurrence(SE); // es, 4

    // Check that stride is non-zero and equal for both accesses
    if (!SE.isKnownNonZero(strideS) || strideS != strideL) {
      
      // Debug Output
      utils::debug("[LF-NDD] --> Strides are different or zero!", 
        utils::YELLOW, "", utils::NO_TERM);
      utils::debugSCEV(" (S = ", strideS, "", "", utils::NO_TERM);
      utils::debugSCEV(" != ", strideL, "", "", utils::NO_TERM);
      utils::debug(" = L)");
      
      return true;
    }

    // Compute the distance between the starting addresses
    // Example: base1 = 0, base2 = 8 -> delta = -8
    const SCEV *delta = SE.getMinusSCEV(baseS, baseL); // base1 - base2

    // Normalize delta according to stride sign
    const SCEV *dependenceDist =
        SE.isKnownNegative(strideS) ? SE.getNegativeSCEV(delta) : delta;

    // Check if the distance is negative
    bool negative = SE.isKnownPredicate(
        ICmpInst::ICMP_SLT, dependenceDist, SE.getZero(delta->getType())
    );

    // Debug Output
    if (negative) {
      utils::debug("[LF-NDD] --> Negative difference! (", 
        utils::YELLOW, "", utils::NO_TERM);
      dyn_cast<SCEVConstant>(delta)->getAPInt().print(outs(), true);
      utils::debug(")");
    }

    return negative;
  }

} // namespace graboidpasses::lf
