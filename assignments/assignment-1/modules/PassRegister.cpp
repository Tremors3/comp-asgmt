//=============================================================================
// FILE:
//    PassRegister.cpp
//
// DESCRIPTION:
//    Register of the three passes.
//
// License: GPL3
//=============================================================================
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "GraboidPasses.hpp"

using namespace llvm;

llvm::PassPluginLibraryInfo getPassRegisterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassRegister", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "algebric-iden") {
                    FPM.addPass(GraboidPasses::AlgebraicIden());
                    return true;
                  }
                  if (Name == "multi-inst-opt") {
                    FPM.addPass(GraboidPasses::MultiInstOpt());
                    return true;
                  }
                  if (Name == "strength-redu") {
                    FPM.addPass(GraboidPasses::StrengthRedu());
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