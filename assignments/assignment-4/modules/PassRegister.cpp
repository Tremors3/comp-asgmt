//==============================================================================
// FILE:
//    PassRegister.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion.
//
// License: GPL3
//==============================================================================
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/TrackingMDRef.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

/*
La Guard del secondo Loop è resa inutile dalla Guard del primo Loop.

Dopo la verifica che entrambi i loop hanno lo stesso trip count:

1. Se N = 0:
La Guard di Loop1 salta l'esecuzione del suo corpo e, se Loop2 è fuso, anche
quello di Loop2. Quindi la Guard di Loop2 non serve più.

2. Se N > 0: Entrambe le Guard valutano a true, ma non hanno effetto perché i
loop verranno comunque eseguiti. Anche in questo caso, la seconda Guard è
inutile.
*/

namespace graboidpasses::lf {

struct LoopFusionPass : PassInfoMixin<LoopFusionPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    return (runOnFunction(F, AM) ? PreservedAnalyses::none()
                                 : PreservedAnalyses::all());
  }

  static bool isRequired() { return true; }

private:
  bool adjacentAnalysis(Loop *FirstLoop, Loop *SecondLoop) {
    outs() << "Current Couple: \n";
    outs() << "\033[1;38:5:255m[LFP-ADJ] First loop:\033[0m "
           << "\033[0;38:5:255m" << FirstLoop->getLoopID()
           << ", Depth: " << FirstLoop->getLoopDepth() << "\033[0m\n"; // DEBUG
    outs() << "\033[1;38:5:255m[LFP-ADJ] Second loop:\033[0m "
           << "\033[0;38:5:255m" << SecondLoop->getLoopID()
           << ", Depth: " << SecondLoop->getLoopDepth() << "\033[0m\n"; // DEBUG

    if (FirstLoop == nullptr || SecondLoop == nullptr)
      return false;

    // Loops must have only one exit block
    SmallVector<BasicBlock *, 1> FL_ExitBlocks;
    SmallVector<BasicBlock *, 1> SL_ExitBlocks;
    FirstLoop->getExitBlocks(FL_ExitBlocks);
    SecondLoop->getExitBlocks(SL_ExitBlocks);
    if (FL_ExitBlocks.size() != 1 || SL_ExitBlocks.size() != 1)
      return false;

    // Basic Blocks must not be null
    BasicBlock *FL_ExitBlock = FirstLoop->getExitBlock();
    BasicBlock *SL_ExitBlock = SecondLoop->getExitBlock();
    BasicBlock *SL_Preheader = SecondLoop->getLoopPreheader();
    if (FL_ExitBlock == nullptr || SL_Preheader == nullptr ||
        SL_Preheader == nullptr)
      return false;

    // Case 1: The Preheader of the second loop is the exit block of the first
    // one
    if (FL_ExitBlock == SL_Preheader) {
      return true;
    }

    // Case 2: The Exit block is the guart or the prehader
    bool isAdjacent = false;
    for (BasicBlock *Succ : successors(FL_ExitBlock)) {
      if (Succ == SL_Preheader || Succ == SL_ExitBlock) {
        isAdjacent = true;
      } else {
        isAdjacent = false;
        break;
      }
    }
    if (isAdjacent) {
      return true;
    }

    // che succede
    // Recursive search on...
    // <llvm/Analysis/CFG.h>
    // if (isPotentiallyReachable(FL_ExitBlock, SL_Preheader, nullptr, &DT)) {
    //   // Controllare i BB intermedi per istruzioni non volute
    //   return true;
    // }

    return false;
  }

  bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
    outs() << "\033[1;38:5:40m[LF] Run on function:\033[0m "
           << "\033[0;38:5:40m" << F.getName() << "\033[0m\n"; // DEBUG
    // Stop Indent

    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

    const std::vector<Loop *> LoopVect = LI.getTopLevelLoops();

    Loop *FL = *LoopVect.begin();
    Loop *SL = *(++LoopVect.begin());

    if (!FL->isLoopSimplifyForm() || !SL->isLoopSimplifyForm())
      return false;

    bool result = adjacentAnalysis(FL, SL) || adjacentAnalysis(SL, FL);

    outs() << "-- Are Adjacent? " << result << '\n';

    return result;
  }
};

} // namespace graboidpasses::lf

llvm::PassPluginLibraryInfo getPassRegisterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassRegister", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "-loop-fusion-pass") {
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
