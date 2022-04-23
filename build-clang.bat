if not exist .\build-clang (mkdir build-clang)
cd build-clang
cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=..\cmake\clang-cl.cmake
pause
