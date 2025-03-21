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
#include <llvm/IR/Instruction.h>
#ifndef MY_PASSES_H
#define MY_PASSES_H

#include "llvm/IR/PassManager.h"
#include <llvm/IR/BasicBlock.h>

using namespace llvm;

namespace GraboidPasses {
    
  struct AlgebraicIden: PassInfoMixin<AlgebraicIden> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    static bool isRequired() { return true; }
    bool runOnFunction(Function &F);
    bool runOnBasicBlock(BasicBlock &BB);
    bool applyAlgebraicIdentity(Instruction &Inst, unsigned opnum1, unsigned opnum2, int64_t identity);
    bool applyBothAlgebraicIdentities(Instruction &Inst, Instruction::BinaryOps InstType, uint64_t identity);
    bool algebraicIdentity(Instruction &Inst);
  };

  struct MultiInstOpt: PassInfoMixin<MultiInstOpt> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    static bool isRequired() { return true; }
  };

  struct StrengthRedu: PassInfoMixin<StrengthRedu> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    static bool isRequired() { return true; }
    bool runOnFunction(Function &F);
    bool runOnBasicBlock(BasicBlock &BB);
    bool strengthReduction(Instruction &Inst);
    bool convertMulToShlWithAdjustment(Instruction &BinInst, unsigned opnum1, unsigned opnum2, unsigned k);
    Instruction* addAdjustInstruction(Instruction *After, Instruction::BinaryOps InstType, Value *Oper1, Value *Oper2);
    Instruction* addShiftLeftInstruction(Instruction *BeforeInst, Value *Oper1, uint64_t ShiftAmount);
  };

} // namespace

#endif // MY_PASSES_H