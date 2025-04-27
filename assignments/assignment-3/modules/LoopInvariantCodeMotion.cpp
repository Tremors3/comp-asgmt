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

namespace {

  struct LoopInvariantCodeMotion: PassInfoMixin<LoopInvariantCodeMotion> {
    
    /*-------------------FUNZIONI NON CORRELATE, D'APPOGGIO-------------------*/

    void static printInstruction(std::string text, Instruction *I, std::string terminator = "\033[0m\n") {
      outs() << text; I->print(outs()); outs() << terminator;  // DEBUG
    }

    /*------------------------------------------------------------------------*/

    std::map<Loop*, std::set<Instruction*>> loopInvariantMap;

    /**
     * Restituisce true se l'istruzione phi contiene reaching definitions 
     * riferite a definizioni strettamente locate al di fuori del loop, 
     * altrimenti restituisce false.
     */
    bool static phiOnlyOutDefs(PHINode *phi, Loop *L) {

      // Itero ciascuna reaching definition.
      for (unsigned j = 0; j < phi->getNumIncomingValues(); ++j) {
        Value *ReachingDef = phi->getIncomingValue(j);

        if (isa<Constant>(ReachingDef)) 
          return false;

        // Tutte le reaching definitions devono devono risiedere al di fuori del
        // loop; se incontriamo anche solo una reaching definition riferita ad 
        // una definizione interna al loop, allora l'istruzione a cui 
        // l'operando appartiene non sarà loop-invariant.
        if (Instruction *DefInst = dyn_cast<Instruction>(ReachingDef))
          if (L->contains(DefInst)) 
            return false;
        
      }

      return true;
    }
    
    /**
     * Restituisce true se il valore V è loop-invariant rispetto al loop L.
     * Se il valore è definito da un'istruzione all'interno del loop, verifica
     * che tale istruzione sia anch'essa loop-invariant oppure una phi con solo
     * valori di ingresso esterni (se phi_check è true).
     */
    static bool isValueLoopInvariant(
      Value *V, 
      Loop *L, 
      std::set<Instruction*> *invariantInstructions, 
      bool phi_check = false)
    {
      // Le costanti sono sempre loop-invariant.
      if (isa<Constant>(V))
        return true;

      if (Instruction *I = dyn_cast<Instruction>(V)) {

        // Se l'istruzione è esterna al loop, il valore è loop-invariant.
        if (!L->contains(I))
          return true;

        // Se è una PHI, può comunque essere loop-invariant se tutti i suoi 
        // input provengono da fuori dal loop (opzionale, phi_check).
        if (auto *Phi = dyn_cast<PHINode>(I)) {
          if (phi_check && phiOnlyOutDefs(Phi, L))
            return true;
          return false;
        }

        // Se l'istruzione è interna e non è loop-invariant, restituisce false.
        if (invariantInstructions->count(I) == 0)
          return false;
      }

      return true;
    }

    /**
     * Restituisce true se il tipo dell'istrizione I è tra quelli considerati 
     * ammissibili per essere marcati come loop-invariant. Se non lo è,
     * viene restituito false.
     */
    bool isInstructionTypeAdmissible(Instruction *I) {

      static const std::set<unsigned> MovableOpcodes = {
        // Operazioni Aritmetiche
        Instruction::Add,
        Instruction::Sub,
        Instruction::Mul,
        Instruction::UDiv,
        Instruction::SDiv,
        // Operazioni logiche
        Instruction::And,
        Instruction::Or,
        Instruction::Xor,
        // Operazioni di Shift
        Instruction::Shl,
        Instruction::LShr,
        Instruction::AShr,
        // Comparazioni
        Instruction::ICmp,
        Instruction::FCmp,
        // Operazioni di Casting
        Instruction::ZExt,
        Instruction::SExt,
        Instruction::Trunc,
        Instruction::BitCast,
        // Caricamenti da memoria
        Instruction::Load
      };

      return MovableOpcodes.count(I->getOpcode()) > 0;
    }

    /**
     * Controlla se l'istruzione corrente è loop-invariant.
     * Se lo è, restituisce true, altrimenti false.
     */
    bool isInstructionLoopInvariant(
      Loop &L, 
      Instruction *currInst, 
      std::set<Instruction*> &invariantInstructionSet)
    {
      if (!isInstructionTypeAdmissible(currInst)) 
        return false;

      for (Use &U : currInst->operands())
        if (!isValueLoopInvariant(U.get(), &L, &invariantInstructionSet))
          return false;

      return true;
    }

    /**
     * Per ciascuna istruzione del loop stabilisce se è loop-invariant.
     * In caso affermativo la aggiunge all'insieme delle istruzioni loop
     * invariant, altrimenti no. 
     */
    void markLoopInvariantInstructions(
      Loop &L, 
      std::set<Instruction*> &invariantInstructionSet)
    {
      for (auto *BB : L.getBlocks())
        for (auto &I : *BB) {
          Instruction *CurrInst = &I;
          
          // Se l'istruzione è loop-invariant, viene aggiunta all'insieme
          if (isInstructionLoopInvariant(L, CurrInst, invariantInstructionSet)) {
            invariantInstructionSet.insert(CurrInst);

            // DEBUG
            printInstruction(
                "\033[1;38:5:033m[LICM-ANALYSIS] Invariant Instruction:\033[0m\t"
                "\033[0;38:5:033m", CurrInst
            );
          }
        }
      
    }

    /*------------------------------------------------------------------------*/

    std::map<Loop*, std::set<Instruction*>> candidateInstructionMap;

    /**
     * Restituisce true se l'istruzione I si trova in un blocco che
     * domina tutte le uscite del loop L, altrimenti false.
     */
    bool instructionDominatesAllExits(Instruction *I, Loop &L, DominatorTree &DT) {
      
      SmallVector<BasicBlock *, 0> ExitBlocks;
      L.getExitBlocks(ExitBlocks);

      for (BasicBlock *BB : ExitBlocks)
        if (!DT.dominates(I->getParent(), BB))
          return false;
      
      return true;
    }

    /**
     * Restituisce true se l'istruzione I è morta al di fuori del loop L. Se 
     * tutti gli user di I sono all'interno del loop, allora I è morta al di 
     * fuori del loop.
     */
    bool isVariableDeadOutsideLoop(Instruction *I, Loop &L) {
      for (auto *U : I->users())
        if (Instruction *UserInst = dyn_cast<Instruction>(U))
          if (!L.contains(UserInst->getParent()))
            return false;
      return true;
    }
    
    /**
     * Se l'istruzione I è utilizzata da una phi all'interno del loop, controlla dove
     * sono definite le reaching definitions della phi. Se sono tutte esterne al loop,
     * allora I è assegnato una sola volta. Altrimenti, se anche solo una p una costante
     * oppure una definizione interna al loop, allora I è assegnato più di una volta.
     */
    bool isValueAssignedOnce(Instruction *I, Loop &L) {

      for (User *User : I->users()) {

        if (isa<PHINode>(User) && L.contains(dyn_cast<Instruction>(User))) {
          PHINode *phi = dyn_cast<PHINode>(User);
          
          for (unsigned j = 0; j < phi->getNumIncomingValues(); ++j) {
            Value *ReachingDef = phi->getIncomingValue(j);
            
            if (isa<Constant>(ReachingDef))
              return false;
            
            if (Instruction *ReachingDefInst = dyn_cast<Instruction>(ReachingDef))
              if (ReachingDefInst != I && L.contains(ReachingDefInst))
                return false;
          }
        }
      }

      return true;
    }
    
    /**
     * Restituisce true se l'istruzione I è definita prima di essere usata.
     */
    bool isDefinedBeforeUse(Instruction *I, Loop &L, DominatorTree &DT) {
      // https://llvm.org/doxygen/classllvm_1_1Instruction.html#a784097fca76abad9e815cf1692de79c4
      BasicBlock *BB = I->getParent(); 
      
      for (User *U : I->users()) {
        Instruction *UserInst = dyn_cast<Instruction>(U);
        BasicBlock *UseBB = UserInst->getParent(); 

        if (L.contains(UseBB) && BB == UseBB) {
          if (UserInst->comesBefore(I))
            return false;
        } else {
          if (!DT.dominates(BB, UseBB))
            return false;
        }
      }

      return true;
    }

    /**
     * Filtra le istruzioni loop-invariant per determinare se sono
     * candidate a essere spostate all'esterno del loop.
     */
    void filterInvariantInstructions(
      Loop &L, 
      DominatorTree &DT, 
      std::set<Instruction*> &invariantInstructionSet, 
      std::set<Instruction*> &candidateInstructionSet) 
    {
      for (Instruction* I : invariantInstructionSet) {
          
        bool assignedOnce     = isValueAssignedOnce(I, L);
        bool definedBeforeUse = isDefinedBeforeUse(I, L, DT);
        bool dominatesExits   = instructionDominatesAllExits(I, L, DT);
        bool deadOutsideLoop  = isVariableDeadOutsideLoop(I, L);

        bool isCandidate = assignedOnce && definedBeforeUse && 
                          (dominatesExits || deadOutsideLoop);

        if (isCandidate) {
          candidateInstructionSet.insert(I);
          
          printInstruction(
            "\033[1;38:5:214m[LICM-FILTERING] Movable Instruction:\033[0m\t"
            "\033[0;38:5:214m", I
          );  // DEBUG
        }
      }
    }

    /*----------------------------MOVE INSTRUCTIONS---------------------------*/
 
    Instruction *getLastInstructionBeforeLoop(Loop &L) {
      
      // Prendo l'header del Loop
      BasicBlock *B = L.getHeader();
      // Prendo l'iteratore dei predecessori del basicblock
      auto it = pred_begin(B);
      // Aumento di uno l'iteratore per prendere il basicblock 
      // immediatamente precedente
      BasicBlock *predecessor = *(++it);
      
      // Restituisco l'ultima istruzione del basicblock
      return &predecessor->back();
    }

    void moveBeforeLoop(Instruction *I, Loop &L) {
      I->moveBefore(getLastInstructionBeforeLoop(L));
    }

    void moveInstructions(Loop &L, 
      std::set<Instruction*> &candidateInstructionSet) {
      for (auto &I : candidateInstructionSet){
        printInstruction("\033[1;38:5:196m[LICM-MOVING] Moved Instruction:"
                         "\033[0m\t\033[0;38:5:196m", I, "");  // DEBUG
          
        moveBeforeLoop(I, L);
          
        printInstruction("   --> ", I);  // DEBUG
      }
    }

    /*------------------------------MANAGE LOOPS------------------------------*/

    /**
     * Per ciascun loop, esegue l'analisi per determinare le istruzioni
     * loop-invariant. In seguito filtra le istruzioni loop-invariant
     * per determinare se sono candidate a essere spostate all'esterno
     * del loop.
     */
    void processLoop(Loop &L, DominatorTree &DT) {
      outs() << "\033[1;38:5:255m[LICM] Run on loop at:\033[0m "
             << "\033[0;38:5:255m" << L.getLoopID()
             << ", Depth: " << L.getLoopDepth() << "\033[0m\n";  // DEBUG
  
      auto &invariantInstructionSet = loopInvariantMap[&L];
      invariantInstructionSet.clear();
  
      auto &candidateInstructionSet = candidateInstructionMap[&L];
      candidateInstructionSet.clear();
  
      markLoopInvariantInstructions(L, invariantInstructionSet);
  
      filterInvariantInstructions(
        L, DT, invariantInstructionSet, candidateInstructionSet);
      
      moveInstructions(L, candidateInstructionSet);
      
      outs() << '\n';
    }
    
    /**
     * Visita ricorsiva (DFS-Postorder) sui loops annidati (sub-loops).
     */
    void iterateSubLevelLoops(Loop &L, DominatorTree &DT) {
      for (auto &nested : L.getSubLoops())
        iterateSubLevelLoops(*nested, DT);
      processLoop(L, DT);
    }
    
    /**
     * Visita iterativa (sequenziale) sui loops di livello più alto (top-loops).
     */
    void iterateTopLevelLoops(LoopInfo &LI, DominatorTree &DT) {
      for (auto &L : LI.getTopLevelLoops())
        iterateSubLevelLoops(*L, DT);
    }

    /*------------------------------------------------------------------------*/

    bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
      outs() << "\033[1;38:5:255m[LICM] Run on function:\033[0m "
             << "\033[0;38:5:255m" << F.getName() << "\033[0m\n";  // DEBUG
      
      LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
      DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
      
      iterateTopLevelLoops(LI, DT);

      return false;
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
                  if (Name == "-licm-pass") {
                    FPM.addPass(LoopInvariantCodeMotion());
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