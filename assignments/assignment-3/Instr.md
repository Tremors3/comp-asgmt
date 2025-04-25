# Instructions for Assignment 3

## Manual execution
If you want to utilize passes manually you should follow the following steps:

### 1. ...

### 2. ...

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
