if not exist .\build-clang (mkdir build-clang)
if exist .\build-clang\CMakeCache.txt del .\build-clang\CMakeCache.txt
cd build-clang
cmake .. -G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE=..\cmake\clang-cl.cmake
pause
