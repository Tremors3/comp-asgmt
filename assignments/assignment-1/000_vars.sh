#!/bin/bash

unset ROOT_LABS_TMP
unset LLVM_DIR_TMP

# Manually select the root labs path
#ROOT_LABS_TMP="$HOME/path/to/root/labs/"

# Manually select the llvm libs folder
#LLVM_DIR_TMP="/usr/lib/llvm-19/lib/"

# Automatically get the script path, if ROOT_LABS_TMP is not set
if [ -z "$ROOT_LABS_TMP" ]; then
    ROOT_LABS_TMP=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
fi

# Automatically get the llvm libs folder, if LLVM_DIR_TMP is not set
if [ -z "$LLVM_DIR_TMP" ]; then
    if command -v llvm-config &> /dev/null; then
        LLVM_DIR_TMP=$(llvm-config --libdir)
    elif command -v llvm-config-19 &> /dev/null; then
        LLVM_DIR_TMP=$(llvm-config-19 --libdir)
    else
        echo "Could not find the llvm lib folder automatically"
        exit 1
    fi
fi

# Print variables
echo "Exporting env variables..."
echo " -- ROOT_LABS=$ROOT_LABS_TMP"
echo " -- LLVM_DIR=$LLVM_DIR_TMP"

# Export variables
export ROOT_LABS="$ROOT_LABS_TMP"
export LLVM_DIR="$LLVM_DIR_TMP"