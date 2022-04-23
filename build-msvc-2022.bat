if not exist .\build-msvc (mkdir build-msvc)
if exist .\build-msvc\CMakeCache.txt del .\build-msvc\CMakeCache.txt
cd build-msvc
cmake .. -G "Visual Studio 17 2022"
pause
