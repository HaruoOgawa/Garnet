mkdir build
del /s /q build\*

@rem CMakefilesListとCMakefilesは別の階層にある必要があるのでCMakefilesListと同階層にbuildを作り
@rem その中でmakefileを作ったり諸々のbuild作業をする必要がある
cd build

@rem Makefileを作る
cmake .

call C:\emsdk\emsdk_env.bat
call emcmake cmake .. -G "MinGW Makefiles"
call ..\nmake.exe

cd ..\

pause