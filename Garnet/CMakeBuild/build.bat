mkdir build
del /s /q build\*

cmake -S . -B build
cmake --build build

pause