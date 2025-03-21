//=============================================================================
// FILE:
//    AlgebraicIden.cpp
//
// DESCRIPTION:
//    Algebraic Identity Pass.
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
   * Function that implement a simple algebraic identity optimization.
   */
  bool AlgebraicIden::applyAlgebraicIdentity(Instruction &Inst, unsigned opnum1, unsigned opnum2, int64_t identity) {
    
    Value *Oper1 = Inst.getOperand(opnum1);
    Value *Oper2 = Inst.getOperand(opnum2);
    
    // To apply the pass at least one of the two operands needs to be an integer constant.
    // Here we are checking if the second operand is an integer constant.
    if (ConstantInt *ConstInt = dyn_cast<ConstantInt>(Oper2)){
      
      int64_t ConstantValue = ConstInt->getSExtValue();
      
      // Checking if the constant value its equal to the identity.
      if (ConstantValue != identity) return false;

      // Replacing all instances of the original instruction with its
      // first operator. By doing so we are completely unlinking the 
      // original instruction.
      Inst.replaceAllUsesWith(Oper1);
      
      return true;
    }
    
    return false;
  }

  /*
   * Function that calls the algebraic identity optimization on 
   * the instruction given as argument in both operand orders.
   */
  bool AlgebraicIden::applyBothAlgebraicIdentities(Instruction &Inst, Instruction::BinaryOps InstType, uint64_t identity)  {
    
    // Checking that the instruction to be the correct type. 
    if (Inst.getOpcode() != InstType) { return false; }

    // Optimize the expression by considering both possible operand orders.
    return AlgebraicIden::applyAlgebraicIdentity(Inst, 0, 1, identity) ||
           AlgebraicIden::applyAlgebraicIdentity(Inst, 1, 0, identity);
  }

  /*
   * Function tries to apply algebraic identity optimization for both 
   * add and mull operations on the instruction given as argument.
   */
  bool AlgebraicIden::algebraicIdentity(Instruction &Inst) {
    return AlgebraicIden::applyBothAlgebraicIdentities(Inst, Instruction::Add, 0) ||
           AlgebraicIden::applyBothAlgebraicIdentities(Inst, Instruction::Mul, 1);
  }

  /*
   * Function that iters the instructions of a basic block
   * and tries to apply an algebraic identity optimization
   * over each instruction.
   */
  bool AlgebraicIden::runOnBasicBlock(BasicBlock &BB) {
    bool Transformed = false;
    
    for (auto &Inst: BB) {
      Transformed |= AlgebraicIden::algebraicIdentity(Inst);
    }
  
    return Transformed;
  }

  /*
   * Function that iters the basic blocks of a function.
   */
  bool AlgebraicIden::runOnFunction(Function &F) {
    bool Transformed = false;

    for (auto &BB: F) {
      Transformed |= AlgebraicIden::runOnBasicBlock(BB);
    }

    return Transformed;
  }

  /*
   * Our entry point.
   */
  PreservedAnalyses AlgebraicIden::run(Function &F, FunctionAnalysisManager &) {
    return (AlgebraicIden::runOnFunction(F) ? PreservedAnalyses::none() : PreservedAnalyses::all());
  }

} // namespace GraboidPasses