//============================================================================//
// FILE:
//    PassRegister.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion.
//
// EXECUTE WITH PASSES:
//    -passes="loop(loop-rotate),function(loop-simplify),function(gb-loop-fusion)"
//
// License: GPL3
//============================================================================//
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/IR/PassManager.h>

#include "LFPassManager.hpp"
#include "Utils.hpp"

using namespace llvm;

namespace graboidpasses::lf {

struct LoopFusionPass : PassInfoMixin<LoopFusionPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    return (runOnFunction(F, AM) ? PreservedAnalyses::none()
                                 : PreservedAnalyses::all());
  }

  static bool isRequired() {
    return true;
  }

private:

  bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
    graboidpasses::lf::LFPassManager lfpm(F, AM);
    lfpm.startPass();
    return lfpm.hasTransformed();
  }
};

} // namespace graboidpasses::lf

llvm::PassPluginLibraryInfo getPassRegisterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassRegister", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "gb-loop-fusion") {
                    FPM.addPass(graboidpasses::lf::LoopFusionPass());
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
