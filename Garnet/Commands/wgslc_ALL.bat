set CurrentDir=%~dp0
set ShaderDir=..\Resources\Shaders
set EXEDir=..\src\Library\Vulkan\bin\

for /R %ShaderDir% %%i in (*.spv) do naga %%~i %%~pi%%~ni.wgsl 

pause