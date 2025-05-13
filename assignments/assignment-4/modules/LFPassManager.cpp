//==============================================================================
// FILE:
//    LFPassManager.cpp
//
// DESCRIPTION:
//    Academic implementation of loop fusion.
//
// License: GPL3
//==============================================================================
#include "LFPassManager.hpp"

using namespace llvm;

namespace graboidpasses::lf {

  /**
   * Entry point.
   */
  void LFPassManager::startPass() {}


  /**
   *
   */
  void LFPassManager::processLoop(Loop &L) {
    outs() << "\033[1;38:5:255m[LF] Run on loop at:\033[0m "
           << "\033[0;38:5:255m" << L.getLoopID()
           << ", Depth: " << L.getLoopDepth() << "\033[0m\n";  // DEBUG

    outs() << '\n';
  }

} // namespace graboidpasses::licm
