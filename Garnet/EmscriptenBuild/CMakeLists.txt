 
cmake_minimum_required(VERSION 3.22.1)

# declare using C and C++
project(Garnet, C CXX)

set(CMAKE_BUILD_TYPE Release)

add_definitions(-D__DAWN__)

add_executable(
	Garnet
	../src/App/CWebAppManager.cpp
	../src/App/CWebAppManager.h
	../src/App/EAppType.h
	../src/App/EditorApp/CEditorApp.cpp
	../src/App/EditorApp/CEditorApp.h
	../src/App/MainApp/CMainApp.cpp
	../src/App/MainApp/CMainApp.h
	../src/App/ScriptApp/CScriptApp.cpp
	../src/App/ScriptApp/CScriptApp.h
	../src/Debug/Message/Console.h
	../src/File/CFileReader.cpp
	../src/File/CFileReader.h
	../src/Graphics/CMaterial.cpp
	../src/Graphics/CMaterial.h
	../src/Graphics/CMesh.cpp
	../src/Graphics/CMesh.h
	../src/Graphics/CPresetPrimitive.cpp
	../src/Graphics/CPresetPrimitive.h
	../src/Graphics/CPrimitive.cpp
	../src/Graphics/CPrimitive.h
	../src/Graphics/CUniformBuffer.cpp
	../src/Graphics/CUniformBuffer.h
	../src/Graphics/CUniformBufferDescriptor.cpp
	../src/Graphics/CUniformBufferDescriptor.h
	../src/Graphics/CVertexBuffer.cpp
	../src/Graphics/CVertexBuffer.h
	../src/Graphics/SPrimitiveCreateInfo.h
	../src/Graphics/SRendererCreateInfo.h
	../src/GraphicsAPI/CRendererCreateInfo.cpp
	../src/GraphicsAPI/CRendererCreateInfo.h
	../src/GraphicsAPI/ERenderPassType.h
	../src/GraphicsAPI/WebGPU/CWebGPUAPI.cpp
	../src/GraphicsAPI/WebGPU/CWebGPUAPI.h
	../src/GraphicsAPI/WebGPU/CWebGPURenderer.cpp
	../src/GraphicsAPI/WebGPU/CWebGPURenderer.h
	../src/Interface/IApp.h
	../src/Interface/IBindingBuffer.h
	../src/Interface/IBindingDescriptor.h
	../src/Interface/IGraphicsAPI.h
	../src/Interface/IRenderer.h
	../src/Main/WebMain/WebMain.cpp
	../src/Math/CTransform.cpp
	../src/Math/CTransform.h
	../src/Object/C3DObject.cpp
	../src/Object/C3DObject.h
	../src/Object/CNode.cpp
	../src/Object/CNode.h
	../src/Object/CObject.cpp
	../src/Object/CObject.h
	../src/Scene/CScriptScene.cpp
	../src/Scene/CScriptScene.h

)

target_include_directories(Garnet PRIVATE ../src/Library/WebGPU ../src/Library/glm)

if(EMSCRIPTEN)
	target_link_options(Garnet PRIVATE 
		-sEXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString','malloc','free']
		-sUSE_WEBGPU=1
		-sALLOW_MEMORY_GROWTH
		-sFETCH
	)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()
