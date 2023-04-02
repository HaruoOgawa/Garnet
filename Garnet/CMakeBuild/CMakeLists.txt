 
cmake_minimum_required(VERSION 3.22.1)

# declare using C and C++
project(Garnet, C CXX)

set(CMAKE_BUILD_TYPE Release)

add_definitions(-D__DAWN__ -D__CMAKE__)

add_executable(
	Garnet
	../src/App/CDescAppManager.cpp
	../src/App/CDescAppManager.h
	../src/App/EAppType.h
	../src/App/EditorApp/CEditorApp.cpp
	../src/App/EditorApp/CEditorApp.h
	../src/App/MainApp/CMainApp.cpp
	../src/App/MainApp/CMainApp.h
	../src/App/ScriptApp/CScriptApp.cpp
	../src/App/ScriptApp/CScriptApp.h
	../src/Debug/Message/Console.h
	../src/File/CFile.cpp
	../src/File/CFile.h
	../src/Graphics/CMaterial.cpp
	../src/Graphics/CMaterial.h
	../src/Graphics/CMesh.cpp
	../src/Graphics/CMesh.h
	../src/Graphics/CPrimitive.cpp
	../src/Graphics/CPrimitive.h
	../src/Graphics/CUniformBufferObject.cpp
	../src/Graphics/CUniformBufferObject.h
	../src/Graphics/CVertexBuffer.cpp
	../src/Graphics/CVertexBuffer.h
	../src/Graphics/SUniformBufferObject.h
	../src/GraphicsAPI/CRendererCreateInfo.cpp
	../src/GraphicsAPI/CRendererCreateInfo.h
	../src/GraphicsAPI/CWebGPUAPI.cpp
	../src/GraphicsAPI/CWebGPUAPI.h
	../src/GraphicsAPI/CWebGPURenderer.cpp
	../src/GraphicsAPI/CWebGPURenderer.h
	../src/GraphicsAPI/ERenderPassType.h
	../src/Interface/IApp.h
	../src/Interface/IGraphicsAPI.h
	../src/Interface/IObject.h
	../src/Interface/IRenderer.h
	../src/Main/DescMain/DescMain.cpp
	../src/Node/CNode.cpp
	../src/Node/CNode.h
	../src/Scene/CScriptScene.cpp
	../src/Scene/CScriptScene.h

	../src/Library/glfw3webgpu/glfw3webgpu.h
	../src/Library/glfw3webgpu/glfw3webgpu.c
)

if(NOT EMSCRIPTEN)
	set_target_properties(
		Garnet PROPERTIES
		CXX_STANDARD 17
		COMPILE_WARNING_AS_ERROR ON
		LINK_FLAGS /SUBSYSTEM:CONSOLE
	)

	target_include_directories(Garnet PRIVATE ../src/Library/GLFW/include ../src/Library/WebGPU ../src/Library/glfw3webgpu ../src/Library/glm)
	target_link_directories(Garnet PRIVATE ../src/Library/GLFW/lib ../src/Library/WebGPU/windows-x86_64)
	target_link_libraries(Garnet glfw3.lib wgpu_native.lib)
else()
	set_target_properties(
		Garnet PROPERTIES
		CXX_STANDARD 17
		COMPILE_WARNING_AS_ERROR ON
	)

	target_include_directories(Garnet PRIVATE ../src/Library/GLFW/include ../src/Library/WebGPU)
	target_link_directories(Garnet PRIVATE ../src/Library/GLFW/lib ../src/Library/WebGPU/windows-x86_64)
endif()

if (MSVC)
	target_compile_options(Garnet PRIVATE /W4)
elseif(EMSCRIPTEN)
	target_link_options(Garnet PRIVATE 
		-sEXPORTED_RUNTIME_METHODS=['ccall']
		-sUSE_GLFW=3
		-sUSE_WEBGPU=1
		-sALLOW_MEMORY_GROWTH
	)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
else()
	target_compile_options(Garnet PRIVATE -Wall -Wextra -pedantic)
endif()
