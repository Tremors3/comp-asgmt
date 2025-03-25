//=============================================================================
// FILE:
//    MultiInstOpt.cpp
//
// DESCRIPTION:
//    Multi-Instruction Optimization Pass.
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
#include <llvm/Support/raw_ostream.h>
#include <cmath>
#include <bits/stdc++.h>
#include "GraboidPasses.hpp"

using namespace llvm;

namespace GraboidPasses {

  // Mapping inverse/counter operations
  std::map<Instruction::BinaryOps, Instruction::BinaryOps> MultiInstOpt::CounterOperandMap {
    {Instruction::Add, Instruction::Sub},
    {Instruction::Sub, Instruction::Add},
    {Instruction::Mul, Instruction::SDiv},
    {Instruction::SDiv, Instruction::Mul}
  };
  
  /*
   * Function that iters all the users of a given instruction 
   * and unlinks the useless ones of the given type.
   */
  bool MultiInstOpt::applyMultiInstruction(Instruction &BinInst, Instruction::BinaryOps UserType, unsigned opnum1, unsigned opnum2) {
    bool Transformed = false;

    // Iterating the users...
    for (auto UserIter : BinInst.users()) {

      if (Instruction *InstUser = dyn_cast<Instruction>(UserIter)) {

        // Checking if the user instruction type is the inverse/counter of 
        // the original instruction. Also checking the second operand of 
        // the user is equal to the one of the original instruction.
        // ps. we dont care if the operand is a integer constant or not,
        // we only care it to be equal to its original counterpart.
        if (InstUser->getOpcode() == UserType && 
          InstUser->getOperand(opnum2) == BinInst.getOperand(opnum2)) {

          // We acknowleged that the user instruction is useless; so 
          // we replace all its uses with the equivalent operand.
          InstUser->replaceAllUsesWith(BinInst.getOperand(opnum1));
          Transformed |= true;
        }
      }
    }

    return Transformed;
  }

  /*
   * Function that tries to apply a multi instruction optimization 
   * over the instruction given as parameter.
   */
  bool MultiInstOpt::multiInstruction(Instruction &Inst) {

    // Ensuring the instruction has exactly two operands.
    // We apply multi-inst optimization on binary instructions.
    if (Inst.getNumOperands() != 2) { return false; }

    // Getting the instruction type and its counter instruction type.
    Instruction::BinaryOps InstType = static_cast<Instruction::BinaryOps>(Inst.getOpcode());
    auto UserTypeIter = CounterOperandMap.find(InstType);
    if (UserTypeIter == CounterOperandMap.end()) { return false; /* if not found */ }
    Instruction::BinaryOps UserType = UserTypeIter->second;

    // Applying the multi instruction optimization over main instruction users.
    return MultiInstOpt::applyMultiInstruction(Inst, UserType, 0, 1);
  }

  /*
   * Function that iters the instructions of a basic block
   * and tries to apply a multiple instruction optimization
   * over each instruction.
   */
  bool MultiInstOpt::runOnBasicBlock(BasicBlock &BB) {
    bool Transformed = false;
    
    for (auto &Inst: BB) {
      Transformed |= MultiInstOpt::multiInstruction(Inst);
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
    return (MultiInstOpt::runOnFunction(F) ? PreservedAnalyses::none() : PreservedAnalyses::all());
  }

} // namespace GraboidPasses