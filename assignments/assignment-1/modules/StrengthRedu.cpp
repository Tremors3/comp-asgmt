//=============================================================================
// FILE:
//    StrengthRedu.cpp
//
// DESCRIPTION:
//    Strength Reduction Pass.
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

int64_t nearestPowerOf2(int64_t n) {
  if (n <= 1) return 1;
  int64_t high = 1;
  while (high <= n) high <<= 1;
  int64_t low = high >> 1;
  return (n - low) < (high - n) ? low : high;
}

namespace GraboidPasses {

  /*
   * Function that adds shl instruction.
   */
  Instruction* StrengthRedu::addShiftLeftInstruction(Instruction *BeforeInst, Value *Oper1, uint64_t ShiftAmount) {

    // Creating the shift costant value and instruction.
    Value *ShiftValue = ConstantInt::get(BeforeInst->getType(), ShiftAmount);
    Instruction *ShlInst = BinaryOperator::Create(Instruction::Shl, Oper1, ShiftValue);

    // Inserting the shift instruction 
    // after the orginal instruction.
    ShlInst->insertAfter(BeforeInst);
    BeforeInst->replaceAllUsesWith(ShlInst);
    
    return ShlInst;
  }

  /*
   * Function adds adjustment instruction.
   */
  Instruction* StrengthRedu::addAdjustInstruction(Instruction *BeforeInst, Instruction::BinaryOps InstType, Value *Oper1, Value *Oper2) {
    
    // Creating the adjustment instruction.
    Instruction *AdjInst = BinaryOperator::Create(InstType, Oper1, Oper2);

    // Inserting the adjustment instruction 
    // after the provided instruction.
    AdjInst->insertAfter(BeforeInst);

    // Update all uses of the After instruction
    // But do not update the uses of After in 
    // the adjustment function.
    for (auto &CurrUse : BeforeInst->uses())
      if (CurrUse.getUser() != AdjInst)
        CurrUse.set(AdjInst);
    
    return AdjInst;
  }

  /*
  * Function that transforms a multiplication instruction into a left shift 
  * instruction, with optional adjustments applied if the integer constant 
  * is not a precise power of two. The parameter k defines the maximum allowed 
  * difference between the constant value and the nearest power of two. 
  * This value also determines the maximum number of adjustments that will 
  * be made to the result of the shift operation.
  */
  bool StrengthRedu::convertMulToShlWithAdjustment(Instruction &BinInst, unsigned opnum1, unsigned opnum2, unsigned k) {

    Value *Oper1 = BinInst.getOperand(opnum1);
    Value *Oper2 = BinInst.getOperand(opnum2);

    // To apply the pass at least one of the two operands needs to be an integer constant.
    // Here we are checking if the second operand is an integer constant.
    if (ConstantInt *ConstInt = dyn_cast<ConstantInt>(Oper2)) {
      
      int64_t ConstantValue = ConstInt->getSExtValue();

      // We ensure the constant value to be greather than one.
      if (ConstantValue <= 1) return false;
      
      int64_t NearestPowerOf2 = nearestPowerOf2(ConstantValue);

      // Calculating the difference between the integer constant value and the nearest power of two.
      // This is useful to check if the difference is in range k allowing us to apply the pass.
      int64_t Difference = ConstantValue - NearestPowerOf2;
      int64_t AbsDifference = std::abs(Difference);

      // We ensure that the difference is within the maximum range specified by k.
      if (AbsDifference > k) return false;

      // Creating the shift instruction. The shift amount is 
      // the base two logarithm of the nearest power of two.
      uint64_t ShiftAmount = static_cast<uint64_t>(std::log2(NearestPowerOf2));
      Instruction* ShlInst = addShiftLeftInstruction(&BinInst, Oper1, ShiftAmount);
      
      // If the integer constant is not an exact power of two we 
      // must apply an adjustment to compensate the difference.
      if (! ConstInt->getValue().isPowerOf2()) {

        // Choosing the operation adjustment type based on the difference.
        Instruction::BinaryOps AdjInstType = (Difference > 0 ? Instruction::Add : Instruction::Sub);

        // Applying the first adjustment.
        Instruction* AdjInst = StrengthRedu::addAdjustInstruction(ShlInst, AdjInstType, ShlInst, Oper1);

        // Other Adjustments if needed (only when k is greather than 1).
        for (unsigned int i = 0; i < k - 1; ++i) {
          AdjInst = StrengthRedu::addAdjustInstruction(AdjInst, AdjInstType, AdjInst, Oper1);
        }
        
      }

      return true;
    
    }

    return false;
  }

  /*
   * Function that tries to apply strength reduction optimization
   * on the instruction given as argument.
   */
  bool StrengthRedu::strengthReduction(Instruction &Inst) {
    
    // To apply the pass we need to make 
    // sure the instruction is of type mul.
    if (Inst.getOpcode() != Instruction::Mul)
      return false;
    
    // Defining the maximum distance between the integer 
    // constant value and the relative power of two value.
    unsigned k = 1;

    // We are certain the instruction is binary 
    // because mul is a binary operator.
    // We need to substitute the mul with a shl in two cases:
    //   1. the integer constant is the first operand;
    //   2. the integer constant is the second operand.
    // We use logic shorting to apply only one of the two cases.
    return convertMulToShlWithAdjustment(Inst, 0, 1, k) || 
           convertMulToShlWithAdjustment(Inst, 1, 0, k);
  }

  /*
   * Function that iters the instructions of a basic block
   * and tries to apply a strength reduction optimization
   * over each instruction.
   */
  bool StrengthRedu::runOnBasicBlock(BasicBlock &BB) {
    bool Transformed = false;

    for (auto &Inst : BB) {
      Transformed |= StrengthRedu::strengthReduction(Inst);
    }
  
    return Transformed;
  }

  /*
   * Function that iters the basic blocks of a function.
   */
  bool StrengthRedu::runOnFunction(Function &F) {
    bool Transformed = false;

    for (auto &BB : F) {
      Transformed |= StrengthRedu::runOnBasicBlock(BB);
    }
  
    return Transformed;
  }

  /*
   * Our entry point.
   */
  PreservedAnalyses StrengthRedu::run(Function &F, FunctionAnalysisManager &) {
    return (StrengthRedu::runOnFunction(F) ? PreservedAnalyses::none() : PreservedAnalyses::all());
  }

} // namespace MyPasses