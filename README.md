# Roxel
An OpenGL and possibly VR experiment because it sounds fun

## Compilation
### Building from Source
#### Linux Enviroment
To compile for Linux, create a build directory and run cmake. EX:
```bash
Roxel/build$ cmake ..
```
To cross-compile for Windows, create a build directory and run cmake with `CMAKE_TOOLCHAIN_FILE` set to `x86_64-w64-mingw32.cmake` from the `cmake` directory. EX:
```bash
Roxel/build$ cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/x86_64-w64-mingw32.cmake ..
```
#### Windows Enviroment
?
