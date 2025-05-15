#include "CustomLoopGuardGetter.hpp"

BasicBlock *CustomLoopGuardGetter::getLoopGuard() const {
  return customLoopGuardRetrieval();
}

BasicBlock *CustomLoopGuardGetter::customLoopGuardRetrieval() const {
  // The function does not assume that the loop is in simplified form

  // Getting the preheader, if it exists. If it there isn't, use the header.
  // Note: a loop can be guarded even if it doesn't have a preheader.
  BasicBlock *EntryBlock = this->loop->getLoopPreheader();
  if (EntryBlock == nullptr)
    EntryBlock = this->loop->getHeader();

  // We count how many external predecessors the "entry" block (header /
  // preheader) has. To be guarded, a loop must have exactly one external
  // predecessor.
  unsigned externalPreds = 0;
  for (BasicBlock *pred : predecessors(EntryBlock))
    if (!this->loop->contains(pred))
      externalPreds++;
  if (externalPreds != 1)
    return nullptr; // incorrect number of predecessors

  // Checking the predecessors of the loop entry block (header / preheader).
  for (BasicBlock *pred : predecessors(EntryBlock)) {
    if (this->loop->contains(pred))
      continue; // the predecessor is inside the loop (a latch).

    // If the external predecessor ends with a conditional branch
    // instruction it could be a guard.
    if (BranchInst *br = dyn_cast<BranchInst>(pred->getTerminator())) {

      if (!br->isConditional())
        continue; // the guard branch inst must be conditional

      // One of the successors must redirect to the header block,
      // the other one must go at the exit of the current loop.
      BasicBlock *succ1 = br->getSuccessor(0);
      BasicBlock *succ2 = br->getSuccessor(1);

      // Checking that one successor points to the loop "entry" block
      // (header / preheader). And the second successor points at an
      // exit block.
      if (checkGuardSuccessors(succ1, succ2, EntryBlock) ||
          checkGuardSuccessors(succ2, succ1, EntryBlock))
        return pred;
    }
  }

  return nullptr;
}

bool CustomLoopGuardGetter::checkGuardSuccessors(
  BasicBlock *First, BasicBlock *Second, BasicBlock *EntryBlock) const {
  // Checking that one of the branch successors is the "entry" block
  // (header / preheader) of the loop. And the other one is an exit
  // block of the loop.
  return (First == EntryBlock && isLoopExitBlock(Second));
}

bool CustomLoopGuardGetter::isLoopExitBlock(BasicBlock *ExitToCheck) const {
  SmallVector<BasicBlock *, 1> ExitBlocks;
  this->loop->getExitBlocks(ExitBlocks);
  for (BasicBlock *Exit : ExitBlocks)
    if (ExitToCheck == Exit)
      return true;
  return false;
}
