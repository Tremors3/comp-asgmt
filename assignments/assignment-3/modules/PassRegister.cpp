//==============================================================================
// FILE:
//    PassRegister.cpp
//
// DESCRIPTION:
//    Academic implementation of loop invariant code motion, LICM, pass.
//
// License: GPL3
//==============================================================================
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

#include "LICMPassManager.hpp"

using namespace llvm;

struct LicmPass: PassInfoMixin<LicmPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    return (runOnFunction(F, AM) ?
      PreservedAnalyses::none() : PreservedAnalyses::all());
  }

  static bool isRequired() {
    return true;
  }

private:
  bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
    outs() << "\033[1;38:5:40m[LICM] Run on function:\033[0m "
           << "\033[0;38:5:40m" << F.getName() << "\033[0m\n";  // DEBUG

    graboidpasses::licm::LicmPassManager lpm(
      &AM.getResult<LoopAnalysis>(F),
      &AM.getResult<DominatorTreeAnalysis>(F)
    );

    lpm.startPass();

    return lpm.hasTransformed();
  }

};

llvm::PassPluginLibraryInfo getPassRegisterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassRegister", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {

            PB.registerPipelineParsingCallback(
              [](StringRef Name, FunctionPassManager &FPM,
                ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "-licm-pass") {
                    FPM.addPass(LicmPass());
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
