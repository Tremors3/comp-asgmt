
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
    Loop *L, std::vector<StoreInst *> &stores, 
    std::vector<LoadInst *> &loads) const {
    
    for (BasicBlock *BB : L->getBlocks()) {
      for (Instruction &I : *BB) {

        if (StoreInst *store = dyn_cast<StoreInst>(&I))
          stores.push_back(store);

        else if (LoadInst *load = dyn_cast<LoadInst>(&I))
          loads.push_back(load);
      }
    }
  }

  /**
   * Checks if the load and store instructions of the two loops are dependent.
   * If they are, then checks if their dependency distance is negative.
   */
  bool NegativeDistanceDependenceAnalysis::areNegativeDistanceDependent() const {

    std::vector<StoreInst *> FirstLoopStores, SecondLoopStores;
    std::vector<LoadInst *> FirstLoopLoads, SecondLoopLoads;

    getLoopInstructionByType(firstLoop, FirstLoopStores, FirstLoopLoads);
    getLoopInstructionByType(secondLoop, SecondLoopStores, SecondLoopLoads);

    // RAW: store (first loop) -> load (second loop)
    for (StoreInst *Store : FirstLoopStores)
      for (LoadInst *Load : SecondLoopLoads)
        if (DI.depends(Store, Load, true) && isDistanceNegativeSE(
          firstLoop, secondLoop, Store, Load))
          return true;

    // WAR: load (first loop) -> store (second loop)
    for (LoadInst *Load : FirstLoopLoads)
      for (StoreInst *Store : SecondLoopStores)
        if (DI.depends(Load, Store, true) && isDistanceNegativeSE(
          firstLoop, secondLoop, Load, Store))
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
   * producer: a[i+8]     -> {%a, +, 4}<for.body>
   * consumer: a[i]       -> {%a, +, 4}<for.body>
   * delta = consumer_base - producer_base = -8 -> negative distance
   */
  bool NegativeDistanceDependenceAnalysis::isDistanceNegativeSE(
    Loop *producerLoop, Loop *consumerLoop,
    Instruction *producer, Instruction *consumer) const {

    // Get polynomial recurrences for both instructions
    const SCEVAddRecExpr *recP = getSCEVAddRecExpr(producerLoop, producer);
    const SCEVAddRecExpr *recC = getSCEVAddRecExpr(consumerLoop, consumer);

    // If recurrence expression is missing assume dependent
    if (!recP || !recC) {
      utils::debug("[LF-NDD] Recurrence expression missing! "
                  "Assuming negative dependent.", utils::YELLOW);
      return true;
    }

    // Check if the memory accesses have the same base pointer (alloca)
    // Example: a[i] and b[i] have different base pointer, so no dependency
    if (SE.getPointerBase(recP) != SE.getPointerBase(recC))
      return false;

    // Debug output
    utils::debug("[LF-NDD] New Producer/Consumer Couple:", utils::YELLOW);
    utils::debugValue("[LF-NDD] --> Producer:", producer, utils::YELLOW, "", '\t');
    utils::debugSCEV("", recP);
    utils::debugValue("[LF-NDD] --> Consumer:", consumer, utils::YELLOW, "", '\t');
    utils::debugSCEV("", recC);

    // Extract base and stride
    const SCEV *baseP   = recP->getStart(); // es, %a
    const SCEV *baseC   = recC->getStart(); // es, %a
    const SCEV *strideP = recP->getStepRecurrence(SE); // es, 4
    const SCEV *strideC = recC->getStepRecurrence(SE); // es, 4

    // Strides must be equal and non-zero
    if (!SE.isKnownNonZero(strideP) || strideP != strideC) {
      
      // Debug Output
      utils::debug("[LF-NDD] --> Strides are different or zero!", 
        utils::YELLOW, "", utils::NO_TERM);
      utils::debugSCEV(" (P = ", strideP, "", "", utils::NO_TERM);
      utils::debugSCEV(" != ", strideC, "", "", utils::NO_TERM);
      utils::debug(" = C)");

      return true;
    }

    // Compute the distance between the starting addresses
    const SCEV *delta = SE.getMinusSCEV(baseC, baseP);

    // Normalize according to stride sign
    const SCEV *dependenceDist =
        SE.isKnownNegative(strideP) ? SE.getNegativeSCEV(delta) : delta;

    // Check if the distance is negative
    bool negative = SE.isKnownPredicate(
        ICmpInst::ICMP_SLT, dependenceDist, SE.getZero(delta->getType())
    );

    // Debug Output
    if (negative) {
      utils::debug("[LF-NDD] --> Negative difference! (", 
        utils::YELLOW, "", utils::NO_TERM);
      if (const auto *C = dyn_cast<SCEVConstant>(delta))
        C->getAPInt().print(outs(), true);
      utils::debug(")");
    }

    return negative;
  }

} // namespace graboidpasses::lf
