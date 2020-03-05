if not exist .\build-mingw (mkdir build-mingw)
if exist .\build-mingw\CMakeCache.txt del .\build-mingw\CMakeCache.txt
cd build-mingw
cmake .. -G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE=..\cmake\gcc.cmake
pause
