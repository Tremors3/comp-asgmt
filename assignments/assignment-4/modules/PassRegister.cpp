//==============================================================================
// FILE:
//    PassRegister.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion.
//
// License: GPL3
//==============================================================================
#include <llvm/ADT/SmallVector.h>
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

  bool isLoopExitBlock(Loop *L, BasicBlock *ExitToCheck) const {
    SmallVector<BasicBlock*, 1> ExitBlocks;
    L->getExitBlocks(ExitBlocks);
    for (BasicBlock *Exit : ExitBlocks)
      if (ExitToCheck == Exit)
        return true;
    return false;
  }

  bool checkGuardSuccessors(Loop *L, BasicBlock *First, BasicBlock *Second, BasicBlock *EntryBlock) const {
    // Checking that one of the branch successors is the entry block
    // (header/preheader) of the loop. And the other one is an exit
    // block of the loop.
    return (First == EntryBlock && isLoopExitBlock(L, Second)); // Second == L->getExitBlock()
  }

  BasicBlock* getLoopGuard(Loop *L) const {

    // Getting the preheader, if it exists. If it there isn't, use the header.
    // Note: a loop can be guarded even if it doesn't have a preheader.
    BasicBlock *EntryBlock = L->getLoopPreheader();
    if (EntryBlock == nullptr)
      EntryBlock = L->getHeader();

    // We count how many external predecessors the entry block (header/preheader)
    // has. To be guarded, a loop must have exactly one external predecessor.
    unsigned externalPreds = 0;

    // Checking the predecessors of the loop entry block (header/preheader).
    for (BasicBlock *pred : predecessors(EntryBlock)) {
      if (L->contains(pred))
        continue; // the predecessor is inside the loop (a latch).

      if (externalPreds++)
        break; // already found an external predecessor.

      // If the external predecessor ends with a conditional branch instruction
      // it could be a guard.
      if (BranchInst *br = dyn_cast<BranchInst>(pred->getTerminator())) {

        if (!br->isConditional())
          continue; // the guard branch inst must be conditional

        // One of the successors must redirect to the header block,
        // the other one must go at the exit of the current loop.
        BasicBlock *succ1 = br->getSuccessor(0);
        BasicBlock *succ2 = br->getSuccessor(1);

        // Checking that one successor points to the loop entry block
        // (header/preheader). And the second successor points at an
        // exit block.
        if (checkGuardSuccessors(L, succ1, succ2, EntryBlock) ||
            checkGuardSuccessors(L, succ2, succ1, EntryBlock))
          return pred;
      }

    }

    return nullptr;
  }

  bool adjacentAnalysis(Loop *FirstLoop, Loop *SecondLoop) {
    outs() << "Current Couple: \n";
    outs() << "\033[1;38:5:255m[LFP-ADJ] First loop:\033[0m "
           << "\033[0;38:5:255m" << FirstLoop->getLoopID()
           << ", Depth: " << FirstLoop->getLoopDepth() << "\033[0m\n"; // DEBUG
    outs() << "\033[1;38:5:255m[LFP-ADJ] Second loop:\033[0m "
           << "\033[0;38:5:255m" << SecondLoop->getLoopID()
           << ", Depth: " << SecondLoop->getLoopDepth() << "\033[0m\n"; // DEBUG

    // Validation
    if (FirstLoop == nullptr || SecondLoop == nullptr)
      return false;

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

    // bool result = adjacentAnalysis(FL, SL) || adjacentAnalysis(SL, FL);
    // outs() << "-- Are Adjacent? " << result << '\n';

    BasicBlock *first = getLoopGuard(FL), *second = getLoopGuard(SL);
    outs() << "First Loop guarded?: " << first << '\n';
    outs() << "Secon Loop guarded?: " << second << '\n';

    return false;
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
