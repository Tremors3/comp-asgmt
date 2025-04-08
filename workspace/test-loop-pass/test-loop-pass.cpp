#include <llvm/IR/Instructions.h>
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
#include <llvm/Analysis/LoopInfo.h>
#include <cmath>
#include <bits/stdc++.h>

using namespace llvm;

namespace {

  struct TestLoopPass: PassInfoMixin<TestLoopPass> {

    void loopTests(Function &F, FunctionAnalysisManager &AM) const {
      LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

      // Point 1
      if (LI.begin() == LI.end()) return ;
      
      // Point 2
      for (auto &BB : F) {
        
        if (LI.isLoopHeader(&BB)) {
          outs() << "BB: ";
          BB.printAsOperand(outs());
          outs() << '\n';
        }

      }

    }

    void printBasicBlock(BasicBlock &BB, std::string text="") const {
      outs() << text;
      BB.printAsOperand(outs());
      outs() << '\n';
    }

    bool isLoopFormCanonical(Loop &L) const {
      // https://llvm.org/docs/LoopTerminology.html#loop-simplify-form
      return ((L.getLoopPreheader() != nullptr)  // Checking if the loop has got a pre-header
        && (L.getNumBackEdges() == 1)  // Checking if the loop has a single backedge and latch 
        && (L.hasDedicatedExits()));  // Checking if the loop has dedicated only exits
    }
    
    void printCFGIterative(Function &F) const {
      
      for (auto &BB : F) {

        printBasicBlock(BB, "Basic Block: ");

        Instruction *terminator = BB.getTerminator();
        if (BranchInst *branch = dyn_cast<BranchInst>(terminator)) {

          if (branch->isConditional()) {
            // Unconditional Branch
            
            BasicBlock *successor = branch->getSuccessor(0);
            printBasicBlock(*successor, "--> Successor: ");

          } else {
            // Conditional Branch

            for (unsigned i = 0; i < branch->getNumSuccessors(); ++i) {
              BasicBlock *successor = branch->getSuccessor(i);
              printBasicBlock(*successor, "--> Successor: ");
            }

          }

        }

      }

    }

    // Point 3
    void loopExercise(Loop &L, LoopInfo &LI) const {

      outs() << '\n';
      
      L.print(outs());
        
      // Subpoint a
      if (isLoopFormCanonical(L))
        outs() << "-- The loop is in canonical form!\n";
      else
        outs() << "-- The loop is NOT in canonical form!\n";

      // Subpoint b
      outs() << "\nFunction CFG:\n";
      // Getting the function handler from the loop header
      Function *Fun = L.getHeader()->getParent();
      printCFGIterative(*Fun);

      // Subpoint c
      outs() << "\nBasic Blocks of the loop:\n";
      for (auto &BB : L.getBlocks())
        printBasicBlock(*BB, "Basic Block: ");

      // Recoursive Nested Loop Prints
      for (auto &nested : L.getSubLoops())
        loopExercise(*nested, LI);

    }

    bool runOnLoop(Function &F, FunctionAnalysisManager &AM) const {
      bool Transformed = false;
      
      LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

      for (auto &L : LI)
        loopExercise(*L, LI);

      return Transformed;
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
      //loopTests(F, AM);
      return (runOnLoop(F, AM) ? PreservedAnalyses::none() : PreservedAnalyses::all());
    }

  };

}

llvm::PassPluginLibraryInfo getPassRegisterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassRegister", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {

            PB.registerPipelineParsingCallback(
              [](StringRef Name, FunctionPassManager &FPM,
                ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "-loop-pass") {
                    FPM.addPass(TestLoopPass());
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