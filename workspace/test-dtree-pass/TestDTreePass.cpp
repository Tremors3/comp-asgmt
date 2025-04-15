#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/GenericDomTree.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/IVDescriptors.h>
#include <llvm/ADT/BreadthFirstIterator.h>
#include <bits/stdc++.h>
#include <cmath>

using namespace llvm;

/** How to establish dominance relationships between BBs, Instructions and Uses
 * https://llvm.org/doxygen/classllvm_1_1DominatorTree.html
 *
 * bool  dominates (const BasicBlock *BB, const Use &U) const
 *  	Return true if the (end of the) basic block BB dominates the use U. 
 *
 * bool  dominates (const Instruction *Def, const BasicBlock *BB) const
 *    Returns true if Def would dominate a use in any instruction in BB. 
 *
 * https://llvm.org/doxygen/classllvm_1_1DominatorTreeBase.html
 *
 * bool  dominates (const DomTreeNodeBase< NodeT > *A, const DomTreeNodeBase< NodeT > *B) const
 *  	dominates - Returns true iff A dominates B.
 *
 * bool  dominates (const NodeT *A, const NodeT *B) const
 *
 * ...
 */

/** How to translate .dot DominatorTree file to .svg
 * https://llvm.org/docs/Passes.html#dot-dom-print-dominance-tree-of-function-to-dot-file
 * 
 * 1. $ opt ... -passes="dot-dom" ...
 * 2. $ dot -Tsvg dom.main.dot -o dom.main.svg
 * 3. $ feh dom.main.svg
 */

namespace {

  struct TestDominatorTreePass: PassInfoMixin<TestDominatorTreePass> {

    void printBasicBlock(BasicBlock *BB, std::string text="") const {
      outs() << text;
      BB->printAsOperand(outs());
      outs() << '\n';
    }
  
    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    void visitDTreeDFSRecursiveStep(DomTreeNodeBase<BasicBlock> *child) const {

      std::string padding(child->getLevel() * 4, ' ');
      printBasicBlock( child->getBlock(), padding + "Node: ");
  
      for (auto subChild : child->children())
        visitDTreeDFSRecursiveStep(subChild);
      
    }

    void visitDTreeDFSRecursive(DomTreeNodeBase<BasicBlock> *root) const {
      visitDTreeDFSRecursiveStep(root);
    }

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    void visitDTreeBFSIterative(DomTreeNodeBase<BasicBlock> *root, std::string text = "Node: ", unsigned pad_size = 4) const {
      
      std::queue<DomTreeNodeBase<BasicBlock>*> queue;
      std::set<DomTreeNodeBase<BasicBlock>*> visited;

      queue.push(root);
      
      while (! queue.empty()) {

        // Getting the next node
        DomTreeNodeBase<BasicBlock> *c = queue.front();
        queue.pop();

        // Marking node as visited
        visited.insert(c);

        // Printing the node
        std::string padding(c->getLevel() * pad_size, ' ');
        printBasicBlock( c->getBlock(), padding + text);

        // Iter the dominated nodes
        for (auto v : c->children()) {

          // Not yet visited
          if (visited.find(v) == visited.end())
            queue.push(v);

        }
      
      }

    }

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    void printTrees(DominatorTree &DT) const {

      outs() << "-- Custom DFS Visit:\n";
      visitDTreeDFSRecursive(DT.getRootNode());

      
      outs() << "\n-- Provided DFS Visit\n";
      for (auto *DTN : depth_first(DT.getRootNode())) {
        std::string padding(DTN->getLevel() * 4, ' ');
        printBasicBlock( DTN->getBlock(), padding + "Node: ");
      }
      

      outs() << "\n-- Custom BFS Visit\n";
      visitDTreeBFSIterative(DT.getRootNode());


      outs() << "\n-- Provided BFS Visit\n";
      for (auto *DTN : breadth_first(DT.getRootNode())) {
        std::string padding(DTN->getLevel() * 4, ' ');
        printBasicBlock( DTN->getBlock(), padding + "Node: ");
      }

    }

    bool runOnFunction(Function &F, FunctionAnalysisManager &AM) const {
      bool Transformed = false;
      
      DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

      printTrees(DT);
      
      return Transformed;
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
      return (runOnFunction(F, AM) ? PreservedAnalyses::none() : PreservedAnalyses::all());
    }

  };

}

llvm::PassPluginLibraryInfo getPassRegisterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassRegister", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {

            PB.registerPipelineParsingCallback(
              [](StringRef Name, FunctionPassManager &FPM,
                ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "-dtree-pass") {
                    FPM.addPass(TestDominatorTreePass());
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