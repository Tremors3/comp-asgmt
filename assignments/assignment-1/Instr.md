# Instructions for Assignment 1 

## Manual execution
If you want to utilize passes manually you should follow the following steps:

### 1. Download 
```bash
git clone https://github.com/Tremors3/comp-asgtm
cd comp-asgtm/assignments/assignment1
```

### 2. If you want to generate the IR from a `.c` file you should use the following command: 
Running clang with optnone flag disabled for `.ll` generation
```bash 
clang -Xclang -disable-O0-optnone -O0 -S -emit-llvm -c "sources/<source>.c" -o "sources/<source>.ll"
```

Converting LLVM IR to SSA form using the mem2reg pass
```bash
opt -p mem2reg "sources/<source>.ll" -o "sources/<source>.bc"
```

Running `llvm-dis` to convert `.bc` to `.ll`
```bash
llvm-dis "sources/<source>.bc" -o "sources/<source>.ll"
```

### 3. Set the environment variables:
```bash
export ROOT_LABS=</path/to>/assignment-1
export LLVM_DIR=</path/to/llvm>/bin
```
### 4. Create the build directory
```bash
mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ../modules
make
cd ..
```
### 5. Run `opt` to apply passes
```bash
opt -load-pass-plugin "build/libAssignment1.so" \
    -passes=<pass_name> "sources/<source>.ll" \
    -o "sources/<source>.optimized.bc"
```
you can chose between these passes:
`algebraic-iden`, `multi-inst-opt`, `strength-redu`. \
You can also apply more
than one pass at the time: 
```bash
-passes="algebric-iden,multi-inst-opt,strength-redu"
```

## Automatic execution
To utilize our passes implementation you can use the bash script we made:
```bash
chmod +x build.sh
```
and then
```bash
./build.sh [<path/to/llvm>/bin]
```
It accepts as an argument the folder location for LLVM if you downloaded it 
manually.
If you installed LLVM system wide by using `apt` or `pacman`, you may not pass
anything since it will automatically search for it.

To chose which test to use you have to edit the `SOURCE_FILE` variable inside
the script (do not specify the file extension).
Then you have to modify which pass to use by commenting or uncommenting the
function calls at the bottom of the script: `execute_passes "..."`.
