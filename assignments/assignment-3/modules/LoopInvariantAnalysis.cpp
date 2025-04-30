//==============================================================================
// FILE:
//    LoopInvariantAnalysis.cpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#include "LoopInvariantAnalysis.hpp"
#include "Utils.hpp"

namespace graboidpasses::licm {

  void LoopInvariantAnalysis::analyzeLoop() {
    if (!loop->isLoopSimplifyForm())
      return;

    for (BasicBlock *BB : loop->getBlocks()) {
      for (Instruction &I : *BB) {
        Instruction *Inst = &I;

        if ( (!visited.count(Inst)) && isInstrLoopInvariant(Inst) ) {
          invariantInstructions.insert(Inst);

          utils::printInstruction(
            "\033[1;38:5:033m[LICM-ANALYSIS] Invariant Instruction:\033[0m\t"
            "\033[0;38:5:033m", Inst
          ); // DEBUG
        }
      }
    }
  }

  bool LoopInvariantAnalysis::isInstrLoopInvariant(Instruction *I) {

    if (visited.count(I))
      return invariantInstructions.count(I);

    visited.insert(I);

    // Limitiamo ai binari per semplicità
    if (!I->isBinaryOp())
      return false;

    for (Value *Operand : I->operands()) {
      if (!isValueLoopInvariant(Operand))
        return false;
    }

    return true;
  }

  bool LoopInvariantAnalysis::isValueLoopInvariant(Value *V) {
    // Costanti: sempre loop-invariant
    if (isa<Constant>(V))
      return true;

    if (Instruction *I = dyn_cast<Instruction>(V)) {
      if (!loop->contains(I))
        return true;

      if (isa<PHINode>(I))
        return false;

      if (!visited.count(I))
        return isInstrLoopInvariant(I);

      return invariantInstructions.count(I);
    }

    return false;
  }

} // namespace graboidpasses::licm
