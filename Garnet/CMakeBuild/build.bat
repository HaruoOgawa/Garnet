mkdir build
del /s /q build\*

cmake -B build
cmake --build build

xcopy ..\WebRelease\sample\Resources build\Debug\Resources /y /s /i

pause