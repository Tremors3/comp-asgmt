//=============================================================================
// FILE:
//    StrengthRedu.cpp
//
// DESCRIPTION:
//    Strength Reduction Pass.
//
// License: GPL3
//=============================================================================
#include <llvm/IR/PassManager.h>
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

/*
 * Gives the nearest power of two of an integer.
 * Rounds up if the distance between the integer
 * and the high and low power of two is the same.
 * Found on Internet.
 */
int64_t nearestPowerOf2(int64_t n) {
  if (n <= 1) return 1;
  int64_t high = 1;
  while (high <= n) high <<= 1;
  int64_t low = high >> 1;
  return (n - low) < (high - n) ? low : high;
}

namespace GraboidPasses {

  /*
   * Function that creates and adds shift instruction.
   */
  Instruction* StrengthRedu::addShiftInstruction(Instruction *BeforeInst, Instruction::BinaryOps ShiftType, Value *Oper1, uint64_t ShiftAmount) {

    // Creating the shift costant value and instruction.
    Value *ShiftValue = ConstantInt::get(BeforeInst->getType(), ShiftAmount);
    Instruction *ShiftInst = BinaryOperator::Create(ShiftType, Oper1, ShiftValue);

    // Inserting the shift instruction 
    // after the orginal instruction.
    ShiftInst->insertAfter(BeforeInst);
    BeforeInst->replaceAllUsesWith(ShiftInst);
    
    return ShiftInst;
  }

  /*
   * Function creates and adds adjustment instruction.
   */
  Instruction* StrengthRedu::addAdjustInstruction(Instruction *BeforeInst, Instruction::BinaryOps InstType, Value *Oper1, Value *Oper2) {
    
    // Creating the adjustment instruction.
    Instruction *AdjInst = BinaryOperator::Create(InstType, Oper1, Oper2);

    // Inserting the adjustment instruction 
    // after the provided instruction.
    AdjInst->insertAfter(BeforeInst);

    // Update all uses of the BeforeInst instruction
    // But do not update the uses of BeforeInst in 
    // the AdjInst instruction.
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

    // To apply the pass we need to make sure the instruction is of type mul.
    if (BinInst.getOpcode() != Instruction::Mul) { return false; }

    Value *Oper1 = BinInst.getOperand(opnum1);
    Value *Oper2 = BinInst.getOperand(opnum2);

    // To apply the pass at least one of the two operands needs to be an integer constant.
    // Here we are checking if the second operand is an integer constant.
    if (ConstantInt *ConstInt = dyn_cast<ConstantInt>(Oper2)) {
      
      int64_t ConstantValue = ConstInt->getSExtValue();

      // We ensure the constant value to be greather than one.
      if (ConstantValue <= 1) { return false; }
      
      int64_t NearestPowerOf2 = nearestPowerOf2(ConstantValue);

      // Calculating the difference between the integer constant value and the nearest power of two.
      // This is useful to check if the difference is in range k allowing us to apply the pass.
      int64_t Difference = ConstantValue - NearestPowerOf2;
      int64_t AbsDifference = std::abs(Difference);

      // We ensure that the difference is within the maximum range specified by k.
      // If k is set to 0 we check if the integer constant is exactly a power of two.
      if (AbsDifference > k) { return false; }

      // Defining the shift type.
      Instruction::BinaryOps ShiftType = Instruction::Shl;

      // The shift amount is the base two logarithm of the nearest power of two.
      uint64_t ShiftAmount = static_cast<uint64_t>(std::log2(NearestPowerOf2));

      // Creating and adding the shift instruction.
      Instruction* ShiftInst = StrengthRedu::addShiftInstruction(&BinInst, ShiftType, Oper1, ShiftAmount);
      
      // If the integer constant is not an exact power of two we 
      // must apply an adjustment to compensate the difference.
      if (! ConstInt->getValue().isPowerOf2()) {

        // Choosing the operation adjustment type based on the difference.
        Instruction::BinaryOps AdjInstType = (Difference > 0 ? Instruction::Add : Instruction::Sub);

        // Applying the first adjustment.
        Instruction* AdjInst = StrengthRedu::addAdjustInstruction(ShiftInst, AdjInstType, ShiftInst, Oper1);

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
   * Function that transforms a division instruction into a right shift instruction, 
   * if the second operator is an integer constant and a precise power of two.
   */
  bool StrengthRedu::convertDivToAShr(Instruction &BinInst) {
    
    // To apply the pass we need to make sure the instruction is of type div.
    if (BinInst.getOpcode() != Instruction::SDiv) { return false; }

    Value *Oper1 = BinInst.getOperand(0);
    Value *Oper2 = BinInst.getOperand(1);

    // To apply the pass at least one of the two operands needs to be an integer constant.
    // Here we are checking if the second operand is an integer constant.
    if (ConstantInt *ConstInt = dyn_cast<ConstantInt>(Oper2)) {

      int64_t ConstantValue = ConstInt->getSExtValue();

      // We ensure the constant value to be greather than one:
      //   1. Division by zero is not allowed;
      //   2. Shifting by zero makes no sense.
      if (ConstantValue <= 1) { return false; }
      
      // Ensuring the second operand is a power of two.
      if (ConstInt->getValue().isPowerOf2()) {
        
        // Defining the shift type.
        Instruction::BinaryOps ShiftType = Instruction::AShr;
        
        // The shift amount is the base two logarithm of the nearest power of two.
        uint64_t ShiftAmount = static_cast<uint64_t>(std::log2(ConstInt->getSExtValue()));
        
        // Creating and adding the shift instruction.
        StrengthRedu::addShiftInstruction(&BinInst, ShiftType, Oper1, ShiftAmount);
        
        return true;
      }
    }

    return false;
  }

  /*
   * Function that manages the Strength Reduction 
   * Optimization for multiplication instructions.
   */
  bool StrengthRedu::strengthReductionMul(Instruction &BinInst) {
    
    // To apply the pass we need to ensure 
    // the instruction is of type mul.
    if (BinInst.getOpcode() != Instruction::Mul) { return false; }
    
    // Defining the maximum distance between the integer 
    // constant value and the relative power of two value.
    unsigned k = 1;
  
    // We need to substitute the mul with a shl in two cases:
    //   1. the integer constant is the first operand;
    //   2. the integer constant is the second operand.
    // We use logic shorting to apply only one of the two cases.
    return StrengthRedu::convertMulToShlWithAdjustment(BinInst, 0, 1, k) || 
           StrengthRedu::convertMulToShlWithAdjustment(BinInst, 1, 0, k);
  }

  /*
   * Function that manages the Strength Reduction 
   * Optimization for division instructions.
   */
  bool StrengthRedu::strengthReductionDiv(Instruction &BinInst) {
    
    // To apply the pass we need to ensure 
    // the instruction is of type sdiv.
    if (BinInst.getOpcode() != Instruction::SDiv) { return false; }
  
    // We want to substitute the sdiv with a ashr if the 
    // second operand is a power of two integer constant.
    return StrengthRedu::convertDivToAShr(BinInst);
  }

  /*
   * Function that tries to apply strength reduction optimization
   * on the instruction given as argument.
   */
  bool StrengthRedu::strengthReduction(Instruction &Inst) {

    // Ensuring the instruction has exactly two operands.
    // We apply strength reduction on binary instructions.
    if (Inst.getNumOperands() != 2) { return false; }

    return StrengthRedu::strengthReductionMul(Inst) || StrengthRedu::strengthReductionDiv(Inst);
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

} // namespace GraboidPasses