if not exist .\build-msvc (mkdir build-msvc)
if exist .\build-msvc\CMakeCache.txt del .\build-msvc\CMakeCache.txt
cd build-msvc
cmake .. -G "Visual Studio 15 2017 Win64"
pause
