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

### PUNTO PRIMO - ADJACENCY

1. Se il primo loop è guarded allora non possiamo fonderlo con un secondo senza guard perchè il corpo del secondo loop non viene per forza saltato se viene saltato il corpo del primo loop.

2. Possiamo fondere i loop solamente in due casi specifici:
    - se entrambi i loop sono guarded e le guard sono tra loro equivalenti (condividono la stessa condizione).
    - se entrambi i loop non sono guarded.

3. I casi particolari in cui dobbiamo verificare la presenza di istruzioni ("in più") rompiscatole sono:
    - quando il preheader del secondo loop non coincide con l'exit block del primo l'oop;
      in questo caso dobbiamo verificare la presenza di istruzioni che non possono essere spostate fuori dai piedi.
    - NOTA: Quindi non dobbiamo iterare ricorsivamente i blocchi.

4. Il prof non richiede lo spostamento delle istruzioni.

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

    // Basic Blocks mustn't be null
    BasicBlock *FL_ExitBlock = FirstLoop->getExitBlock();
    BasicBlock *SL_ExitBlock = SecondLoop->getExitBlock();
    BasicBlock *SL_Preheader = SecondLoop->getLoopPreheader();
    if (FL_ExitBlock == nullptr || SL_Preheader == nullptr ||
        SL_Preheader == nullptr)
      return false;

    // Case 1: The Preheader of the second loop
    // is the exit block of first loop
    if (FL_ExitBlock == SL_Preheader) {
      return true;
    }

    // Getting the successor of the first loop exit
    BasicBlock *FL_ExitSuccessor = FL_ExitBlock->getSingleSuccessor();

    // Case 2: The Preheader of the second loop is
    // the successor of the exit block of first loop
    if (FL_ExitBlock == SL_Preheader) {

      // TODO: Check for instructions in the second loop preheader

      return true;
    }

    // Case 3: The Guard of the second loop is
    // the Exit block is the guard or the prehader
    bool isGuarded = false; // second loop is guarded
    for (BasicBlock *Succ : successors(FL_ExitSuccessor)) {
      if (Succ == SL_Preheader || Succ == SL_ExitBlock) {
        isGuarded = true;
      } else {
        isGuarded = false;
        break;
      }
    }
    if (isGuarded) { // true if the second loop is guarded

      // TODO: Check for instructions in the second loop preheader

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
