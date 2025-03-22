//=============================================================================
// FILE:
//    GraboidPasses.hpp
//
// DESCRIPTION:
//    Header file that prototypes the three passes.
//
// License: GPL3
//=============================================================================
#pragma once
#include <cstdint>
#ifndef GRABOID_PASSES_H
#define GRABOID_PASSES_H

#include "llvm/IR/PassManager.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <bits/stdc++.h>

using namespace llvm;

namespace GraboidPasses {
    
  struct AlgebraicIden: PassInfoMixin<AlgebraicIden> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    static bool isRequired() { return true; }

    // Iterate Functions
    bool runOnFunction(Function &F);
    bool runOnBasicBlock(BasicBlock &BB);

    // Algebric Identity Manager
    bool algebraicIdentity(Instruction &Inst);
    
    // Algebric Identity Applicators
    bool applyAlgebraicIdentity(Instruction &BinInst, unsigned opnum1, unsigned opnum2, int64_t identity);
    bool applyBothAlgebraicIdentities(Instruction &BinInst, Instruction::BinaryOps InstType, uint64_t identity);
  };

  struct MultiInstOpt: PassInfoMixin<MultiInstOpt> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    static bool isRequired() { return true; }

    // Mapping inverse/counter operations
    static std::map<Instruction::BinaryOps, Instruction::BinaryOps> CounterOperandMap;

    // Iterate Functions
    bool runOnFunction(Function &F);
    bool runOnBasicBlock(BasicBlock &BB);

    // Multi Instruction Optimization Manager
    bool multiInstruction(Instruction &Inst);
    
    // Multi Instruction Optimization Applicator
    bool applyMultiInstruction(Instruction &BinInst, Instruction::BinaryOps UserType, unsigned opnum1, unsigned opnum2);
  };

  struct StrengthRedu: PassInfoMixin<StrengthRedu> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    static bool isRequired() { return true; }
    
    // Iterate Functions
    bool runOnFunction(Function &F);
    bool runOnBasicBlock(BasicBlock &BB);
    
    // Strength Reduction Managers
    bool strengthReduction(Instruction &Inst);
    bool strengthReductionMul(Instruction &BinInst);
    bool strengthReductionDiv(Instruction &BinInst);
    
    // Strength Reduction Applicators
    bool convertDivToAShr(Instruction &BinInst);
    bool convertMulToShlWithAdjustment(Instruction &BinInst, unsigned opnum1, unsigned opnum2, unsigned k);
    
    // Support Functions
    Instruction* addAdjustInstruction(Instruction *BeforeInst, Instruction::BinaryOps InstType, Value *Oper1, Value *Oper2);
    Instruction* addShiftInstruction(Instruction *BeforeInst, Instruction::BinaryOps ShiftType, Value *Oper1, uint64_t ShiftAmount);
  };

} // namespace GraboidPasses

#endif // GRABOID_PASSES_H