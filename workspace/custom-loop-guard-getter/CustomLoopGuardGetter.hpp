#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/TrackingMDRef.h>
#include <llvm/IR/Value.h>

using namespace llvm;

struct CustomLoopGuardGetter {
public:
  CustomLoopGuardGetter(Loop *L) : loop(L) {}

  BasicBlock *getLoopGuard() const;

private:
  const Loop *loop;

  BasicBlock *customLoopGuardRetrieval() const;

  bool checkGuardSuccessors(BasicBlock *First, BasicBlock *Second,
                            BasicBlock *EntryBlock) const;

  bool isLoopExitBlock(BasicBlock *ExitToCheck) const;
};
