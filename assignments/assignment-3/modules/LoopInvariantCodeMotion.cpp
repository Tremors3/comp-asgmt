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

    bool static isInstructionInsideLoop(Instruction *I, Loop *L) {
      BasicBlock *PBB = I->getParent();
      return L->contains(PBB);
    }

    void static printInstruction(std::string text, Instruction *I) {
      outs() << text; I->print(outs()); outs() << "\033[0m\n";  // DEBUG
    }

    /*------------------------------------------------------------------------*/

    std::map<Loop*, std::set<Instruction*>> loopInvariantMap;

    /**
     * Restituisce true se l'istruzione I è movibile rispetto al loop L,
     * altrimenti restituisce false.
     */
    bool isMovableInstruction(Instruction *I) {
      // Set di opcode che rappresentano istruzioni spostabili
      static const std::set<unsigned> MovableOpcodes = {
        // Aritmetica
        Instruction::Add,
        Instruction::Sub,
        Instruction::Mul,
        Instruction::UDiv,
        Instruction::SDiv,
        
        // Operazioni logiche
        Instruction::And,
        Instruction::Or,
        Instruction::Xor,
        
        // Shift
        Instruction::Shl,
        Instruction::LShr,
        Instruction::AShr,
        
        // Comparazioni
        Instruction::ICmp,
        Instruction::FCmp,
        
        // Cast
        Instruction::ZExt,
        Instruction::SExt,
        Instruction::Trunc,
        Instruction::BitCast,
        
        // Caricamenti da memoria
        Instruction::Load
      };
    
      // Controlla se l'istruzione è nel set di opcode spostabili
      return MovableOpcodes.count(I->getOpcode()) > 0;
    }

    /**
     * Restituisce true se l'istruzione phi contiene reaching definitions 
     * riferite a definizioni strettamente locate al di fuori del loop, 
     * altrimenti restituisce false.
     */
    bool static phiOnlyOutDefs(PHINode *phi, Loop *L) {

      // Itero ciascuna reaching definition argomento dell'istruzione phi
      for (unsigned j = 0; j < phi->getNumIncomingValues(); ++j) {
        Value *incoming = phi->getIncomingValue(j);

        // Le costanti sono definite internamente alla phi, all'interno del loop
        if (isa<Constant>(incoming)) { return false; }

        // Tutte le reaching definitions devono devono risiedere al di fuori del loop;
        // se incontriamo anche solo una reaching definition riferita ad una definizione
        // interna al loop, allora l'istruzione a cui l'operatore appartiene non sarà
        // loop-invariant.
        if (Instruction *defInst = dyn_cast<Instruction>(incoming)) {
          if (isInstructionInsideLoop(defInst, L)) { return false; }
        }
      }

      return true;
    }
    
    /**
     * Controlla che il valore V sia loop-invariant rispetto al loop L. 
     * Se V risulta loop-invariant viene restituito true, in caso contrairo false.
     */
    bool static isValueLoopInvariant(Value *V, Loop *L, 
      std::set<Instruction*> *invariantInstructions, bool phi_check = false) {
      
      // Le costanti intere sono loop-invariant.
      if (isa<Constant>(V)) { return true; }

      if (Instruction *I = dyn_cast<Instruction>(V)) {

        // Se l'istruzione che definisce il valore V si trova 
        // esterna al loop L, allora V è loop-invariant.
        if (!isInstructionInsideLoop(I, L)) { return true; }
  
        // Se l'istruzione che definisce il valore V si trova 
        // interna del loop L, dobbiamo verificare alcune cose.
        if (isInstructionInsideLoop(I, L)) {

          // Se l'istruzione è una phi (il valore V 
          // ha più di una possibile definizione)
          if (auto *phi = dyn_cast<PHINode>(I)) {
            // Anche se ci sono più definizioni dentro al loop,
            // è possibile che tutte provengano da fuori il loop:
            // in questo caso, il valore è comunque loop-invariant.
            // 
            // In pratica: l'istruzione ha un operando con più definizioni
            // (quindi c'è una phi), ma tutte queste definizioni
            // vengono da fuori il loop.
            // 
            // In realtà LLVM di solito mette le phi nel preheader,
            // quindi è raro avere phi dentro il corpo del loop.
            // 
            // Nota: se spostiamo un'istruzione nel preheader,
            // dovremmo anche spostare la phi, se è dentro al loop
            // ma fa riferimento solo a valori esterni.
            // Questa operazione però è un po' complicata.
            if (phi_check && phiOnlyOutDefs(phi, L)) { return true; }
            
            return false;
          }

          // Se l'istruzione non è tra quelle loop-invariant
          if (invariantInstructions->count(I) == 0) {
            return false;
          }
        }
      }

      return true;
    }

    /**
     * Controlla se l'istruzione corrente è loop-invariant.
     * Se lo è, restituisce true, altrimenti false.
     */
    bool isInstructionLoopInvariant(Loop &L, Instruction *currInst, 
      std::set<Instruction*> &invariantInstructionSet){
      
      if (!isMovableInstruction(currInst)) { return false; }

      // Controlliamo che ciascun operando sia loop-invariant.
      for (Use &U : currInst->operands()) {
        if (!isValueLoopInvariant(U.get(), &L, &invariantInstructionSet)) {
          return false;
        }
      }

      // Se anche solo un operando non è risultato loop-invariant
      // allora l'intera istruzione non è loop-invariant.
      return true;
    }

    /**
     * Per ciascuna istruzione del loop stabilisce se è loop-invariant.
     * In caso affermativo la aggiunge all'insieme delle istruzioni loop
     * invariant, altrimenti no. 
     */
    void markLoopInvariantInstructions(Loop &L, 
      std::set<Instruction*> &invariantInstructionSet) {
      
      // Itera i BasicBlock del loop
      for (auto *BB : L.getBlocks()) {

        // Itera le istruzioni del BasickBlock
        for (auto &I : *BB) {
          Instruction *currInst = &I;
          
          // Se l'istruzione e' loop-invariant viene aggiunta all'insieme delle
          // istruzioni loop invariant.
          if (isInstructionLoopInvariant(L, currInst, invariantInstructionSet)) {
            invariantInstructionSet.insert(currInst);
            printInstruction("\033[1;38:5:033m[LICM-ANALYSIS] Invariant Instruction:\033[0m\t\033[0;38:5:033m", &I);  // DEBUG
          }
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
      
      BasicBlock *PBB = I->getParent();
      SmallVector<BasicBlock *, 0> ExitBlocks;
      
      L.getExitBlocks(ExitBlocks);
      
      for (BasicBlock *BB : ExitBlocks) {
        if (!DT.dominates(PBB, BB)) { return false; }
      }
      
      return true;
    }

    bool variableIsDeadOutsideLoop(Instruction *I, Loop &L) {
      // Itera tutti gli user di una istruzione
      for (auto *User : I->users()) {
        if (Instruction *UserInst = dyn_cast<Instruction>(User)) {
          // Controlla se l'user e' fuori dal loop
          if (!L.contains(UserInst->getParent())) { return false; }
        }
      }

      return true;
    }
    
    bool isValueAssignedOnce() {
      return true;  // TODO: implementare
    }
    
    bool isDefinedBeforeUse(){
      return true;  // TODO: implementare
    }
    
    /**
     * Filtra le istruzioni loop-invariant per determinare se sono
     * candidate a essere spostate all'esterno del loop.
     */
    void filterInvariantInstructions(Loop &L, DominatorTree &DT, 
      std::set<Instruction*> &invariantInstructionSet, 
      std::set<Instruction*> &candidateInstructionSet) {

      for (auto InvarInst : invariantInstructionSet) {
        Instruction* I = InvarInst;

        bool isCandidate = \
          
          // Filtro sulla tipologia di istruzione
          isMovableInstruction(I) &&

          // Filtri sull'istruzione
          isValueAssignedOnce() &&
          isDefinedBeforeUse() && (
            instructionDominatesAllExits(I, L, DT) ||
            variableIsDeadOutsideLoop(I, L)
          );

        if (isCandidate) {
          candidateInstructionSet.insert(I);
          printInstruction("\033[1;38:5:214m[LICM-FILTERING] Movable Instruction:\033[0m\t\033[0;38:5:214m", I);  // DEBUG
        }
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

      // Questo controllo non è strettamente necessario.
      //if (!L.isLoopSimplifyForm()) return;

      outs() << "\033[1;38:5:255m[LICM] Run on loop at:\033[0m \033[0;38:5:255m"
             << L.getLoopID() 
             << ", Depth: " 
             << L.getLoopDepth() << "\033[0m\n";  // DEBUG
      
      // Ottengo una referenza all'insieme di istruzioni marcate
      // loop-invariant riferite al loop corrente L. Inizializzo
      // l'insieme vuoto in modo da pulirlo da sporcizia lasciata
      // da eventuali run passate.
      auto &invariantInstructionSet = loopInvariantMap[&L];
      invariantInstructionSet = {};

      auto &candidateInstructionSet = candidateInstructionMap[&L];
      candidateInstructionSet = {};

      markLoopInvariantInstructions(L, invariantInstructionSet);

      filterInvariantInstructions(
        L, DT, invariantInstructionSet, candidateInstructionSet);

      outs() << '\n';
    }
    
    /**
     * Visita ricorsiva (DFS-Postorder) sui loops annidati (sub-loops).
     */
    void iterateSubLevelLoops(Loop &L, DominatorTree &DT) {
      for (auto &nested : L.getSubLoops()) {
        iterateSubLevelLoops(*nested, DT);
      }
      processLoop(L, DT);
    }
    
    /**
     * Visita iterativa (sequenziale) sui loops di livello più alto (top-loops).
     */
    void iterateTopLevelLoops(LoopInfo &LI, DominatorTree &DT) {
      for (auto &L : LI.getTopLevelLoops()) {
        iterateSubLevelLoops(*L, DT);
      }
    }

    /*------------------------------------------------------------------------*/

    bool runOnFunction(Function &F, FunctionAnalysisManager &AM) {
      outs() << "\033[1;38:5:255m[LICM] Run on function:\033[0m \033[0;38:5:255m" 
             << F.getName() 
             << "\033[0m\n";  // DEBUG
      
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