#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
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

using namespace llvm;

namespace {

  struct TestFunctionPass: PassInfoMixin<TestFunctionPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {

      outs() << "Function Name: " << F.getName() << '\n';

      return (true ? PreservedAnalyses::none() : PreservedAnalyses::all());
    }
  };

  struct TestModulePass: PassInfoMixin<TestModulePass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {

      outs() << "Module Name: " << M.getName() << '\n';

      for (auto &F : M) {
        outs() << "  -- Function Name: " << F.getName() << '\n';
      }

      return (true ? PreservedAnalyses::none() : PreservedAnalyses::all());
    }
  };
  
}

llvm::PassPluginLibraryInfo getPassRegisterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassRegister", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {

            // REGISTER FUNCTION PASSES
            PB.registerPipelineParsingCallback(
              [](StringRef Name, FunctionPassManager &FPM,
                ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "test-function-pass") {
                    FPM.addPass(TestFunctionPass());
                    return true;
                  }
                  return false;
                });
            
            // REGISTER MODULE PASSES
            PB.registerPipelineParsingCallback(
              [](StringRef Name, ModulePassManager &MPM,
                ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "test-module-pass") {
                    MPM.addPass(TestModulePass());
                    return true;
                  }
                  return false;
                });
          
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getPassRegisterPluginInfo();
}