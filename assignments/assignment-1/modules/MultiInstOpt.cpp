//=============================================================================
// FILE:
//    MultiInstOpt.cpp
//
// DESCRIPTION:
//    Multi-Instruction Optimization Pass.
//
// License: GPL3
//=============================================================================
#include "llvm/IR/PassManager.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/MathExtras.h>
#include <cmath>
#include "GraboidPasses.hpp"

using namespace llvm;

namespace GraboidPasses {

  /*
   * Functiot that optimizes multiple instruction that negate each others:
   * 𝑎 = 𝑏 + 1, 𝑐 = 𝑎 − 1  ⇒  𝑎 = 𝑏 + 1, 𝑐 = 𝑏
   */
  bool MultiInstOpt::optimizeInstructions(Instruction *iterInst, Instruction &Inst, Instruction::BinaryOps InstType) {
      // Check if the user is a subtraction and if the second therm negates
      // each other
      if (iterInst->getOpcode() == InstType && 
          Inst.getOperand(1) == iterInst->getOperand(1)) {
        
        // Replace the uses
        iterInst->replaceAllUsesWith(Inst.getOperand(0));
        return true;
      }
      
    return false;
  }
  
  bool MultiInstOpt::multiInstructionOptimization(Instruction &Inst) {
    Instruction::BinaryOps InstType;
    // Check if operation is an addition
    if (Inst.getOpcode() == Instruction::Add) {
      InstType = Instruction::Sub;

    } else if (Inst.getOpcode() == Instruction::Sub) {
      InstType = Instruction::Add;
    
    } else {
      return false; 
    }

    bool Transformed = false;
    
    // Iterate each user of the instruction
    for (auto userIter = Inst.user_begin(); 
         userIter != Inst.user_end(); ++userIter) {
      
      // Check for successfull cast
      if (Instruction *iterInst = dyn_cast<Instruction>(*userIter)){
        Transformed |= MultiInstOpt::optimizeInstructions(iterInst, Inst, InstType);
    
      } else { Transformed = false; }
    }

    return Transformed;
  }

  /*
   * Function that iters the instructions of a basic block
   * and tries to apply a multiple instruction optimization
   * over each instruction.
   */
  bool MultiInstOpt::runOnBasicBlock(BasicBlock &BB) {
    bool Transformed = false;
    
    for (auto &Inst: BB) {
      Transformed |= MultiInstOpt::multiInstructionOptimization(Inst);
    }
  
    return Transformed;
  }
  
  /*
   * Function that iters the basic blocks of a function.
   */
  bool MultiInstOpt::runOnFunction(Function &F) {
    bool Transformed = false;

    for (auto &BB: F) {
      Transformed |= MultiInstOpt::runOnBasicBlock(BB);
    }

    return Transformed;
  }

   /*
   * Our entry point.
   */
  PreservedAnalyses MultiInstOpt::run(Function &F, FunctionAnalysisManager &) {

    outs() << "PASSO MultiInstOpt\n";

    return (MultiInstOpt::runOnFunction(F) ? PreservedAnalyses::none() : PreservedAnalyses::all());
  }

} // namespace GraboidPasses