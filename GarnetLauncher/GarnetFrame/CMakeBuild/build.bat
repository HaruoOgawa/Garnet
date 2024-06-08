mkdir build
del /s /q build\*

cmake -B build
cmake --build build

xcopy ..\Resources build\Debug\Resources /y /s /i
xcopy ..\src\Library\WebGPU\windows-x86_64\wgpu_native.dll build\Debug/y /s /i

xcopy build\Debug ..\DescRelease /y /s /i

pause