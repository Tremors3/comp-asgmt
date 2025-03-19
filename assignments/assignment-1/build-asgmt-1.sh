#!/bin/bash

# Function to check if a command exists
check_command() {
    if ! command -v "$1" &> /dev/null; then
        echo "$1 is not installed. Install it before running the script again."
        exit 1
    fi
}

# Function to create directories if they do not exist
check_create_dir() {
    if [ ! -e "$1" ]; then
        mkdir "$1" || { echo "Failed to create $1 directory."; exit 1; }
    fi
}

# Dependencies
echo "Dependency check..."
check_command cmake
check_command make
check_command opt
check_command clang
check_command llvm-dis

# Load Env Variables
ENV_VARS_FILE_PATH="./000_vars.sh"
if [ ! -f "$ENV_VARS_FILE_PATH" ]; then
    echo "Could not find $ENV_VARS_FILE_PATH file."
    exit 1
fi
source "$ENV_VARS_FILE_PATH"

# Check if ROOT_LABS is set
if [ -z "$ROOT_LABS" ]; then
    echo "ROOT_LABS is not set. Please check the $ENV_VARS_FILE_PATH script."
    exit 1
fi

# Paths
SOURCES_DIR="$ROOT_LABS/sources"
MODULES_DIR="$ROOT_LABS/modules"
BUILD_DIR="$ROOT_LABS/build"

# Check dirs
check_create_dir "$SOURCES_DIR"
check_create_dir "$MODULES_DIR"
check_create_dir "$BUILD_DIR"

# Source File
SOURCE_FILE="Loop"  # EDIT
SOURCE_FILE_PATH="$SOURCES_DIR/$SOURCE_FILE"

# Building
cd "$BUILD_DIR"
echo "Running cmake..."
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR "$MODULES_DIR"  # EDIT
echo "Running make..."
make
cd ..

# Processing
echo "Running clang for .ll generation..."
clang -O2 -emit-llvm -S -c "$SOURCE_FILE_PATH.c" -o "$SOURCE_FILE_PATH.ll"  # EDIT

echo "Running clang for .bc generation..."
clang -O2 -emit-llvm -c "$SOURCE_FILE_PATH.c" -o "$SOURCE_FILE_PATH.bc"  # EDIT

echo "Running llvm-dis to convert .bc to .ll..."
llvm-dis "$SOURCE_FILE_PATH.bc" -o="$SOURCE_FILE_PATH.ll"  # EDIT

echo "Running opt to apply pass..."
echo
opt -load-pass-plugin "$BUILD_DIR/libLocalOpts.so" \
    -passes="local-opts" "$SOURCE_FILE_PATH.bc" \
    -o "${SOURCE_FILE_PATH}Assigment1.bc"  # EDIT

echo "Build and processing completed successfully."
