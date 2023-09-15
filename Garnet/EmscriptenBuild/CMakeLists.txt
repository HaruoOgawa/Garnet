 
cmake_minimum_required(VERSION 3.22.1)

# declare using C and C++
project(Garnet, C CXX)

set(CMAKE_BUILD_TYPE Release)

add_definitions(
	-DUSE_WEBGPU
	-DUSE_TEXTURE_LOADER
	-DUSE_GLTF
	-DUSE_VIEWER_CAMERA
	-DUSE_INPUT_SYSTEM
	-DUSE_GPGPU
)

add_executable(
	Garnet
	../src/App/CWebAppManager.cpp
	../src/App/CWebAppManager.h
	../src/App/EAppType.h
	../src/App/ScriptApp/CScriptApp.cpp
	../src/App/ScriptApp/CScriptApp.h
	../src/Camera/CCamera.cpp
	../src/Camera/CCamera.h
	../src/Camera/CViewerCamera.cpp
	../src/Camera/CViewerCamera.h
	../src/Debug/Message/Console.h
	../src/File/CFile.cpp
	../src/File/CFile.h
	../src/GLTF/CGLTFImporter.cpp
	../src/GLTF/CGLTFImporter.h
	../src/Graphics/CDrawInfo.cpp
	../src/Graphics/CDrawInfo.h
	../src/Graphics/CMaterial.cpp
	../src/Graphics/CMaterial.h
	../src/Graphics/CMesh.cpp
	../src/Graphics/CMesh.h
	../src/Graphics/CPresetPrimitive.cpp
	../src/Graphics/CPresetPrimitive.h
	../src/Graphics/CPrimitive.cpp
	../src/Graphics/CPrimitive.h
	../src/Graphics/CShaderBuffer.cpp
	../src/Graphics/CShaderBuffer.h
	../src/Graphics/CShaderBufferDescriptor.cpp
	../src/Graphics/CShaderBufferDescriptor.h
	../src/Graphics/CTexture.cpp
	../src/Graphics/CTexture.h
	../src/Graphics/EBlendType.h
	../src/Graphics/ECullMode.h
	../src/Graphics/ETextureType.h
	../src/Graphics/SBindingLayout.h
	../src/Graphics/STextureBindingLayout.h
	../src/GraphicsAPI/CMaterialCreateInfo.cpp
	../src/GraphicsAPI/CMaterialCreateInfo.h
	../src/GraphicsAPI/CRendererCreateInfo.cpp
	../src/GraphicsAPI/CRendererCreateInfo.h
	../src/GraphicsAPI/ERenderPassFormat.h
	../src/GraphicsAPI/WebGPU/CWebGPUAPI.cpp
	../src/GraphicsAPI/WebGPU/CWebGPUAPI.h
	../src/GraphicsAPI/WebGPU/CWebGPUGPGPUHandler.cpp
	../src/GraphicsAPI/WebGPU/CWebGPUGPGPUHandler.h
	../src/GraphicsAPI/WebGPU/CWebGPUMaterial.cpp
	../src/GraphicsAPI/WebGPU/CWebGPUMaterial.h
	../src/GraphicsAPI/WebGPU/CWebGPURenderer.cpp
	../src/GraphicsAPI/WebGPU/CWebGPURenderer.h
	../src/GraphicsAPI/WebGPU/CWebGPURenderPass.cpp
	../src/GraphicsAPI/WebGPU/CWebGPURenderPass.h
	../src/GraphicsAPI/WebGPU/CWebGPUTexture.cpp
	../src/GraphicsAPI/WebGPU/CWebGPUTexture.h
	../src/ImageEffect/CBlurEffect.cpp
	../src/ImageEffect/CBlurEffect.h
	../src/Input/CInputState.cpp
	../src/Input/CInputState.h
	../src/Interface/IApp.h
	../src/Interface/IBuffer.h
	../src/Interface/IDescriptor.h
	../src/Interface/IGPGPUHandler.h
	../src/Interface/IGraphicsAPI.h
	../src/Interface/IRenderer.h
	../src/Interface/IRenderPass.h
	../src/Interface/IResource.h
	../src/LoadWorker/CLoadWorker.cpp
	../src/LoadWorker/CLoadWorker.h
	../src/LoadWorker/ELoadStatus.h
	../src/Main/WebMain/WebMain.cpp
	../src/Math/CMath.h
	../src/Math/CTransform.cpp
	../src/Math/CTransform.h
	../src/Object/C3DObject.cpp
	../src/Object/C3DObject.h
	../src/Object/CNode.cpp
	../src/Object/CNode.h
	../src/Object/CSimple3DObject.cpp
	../src/Object/CSimple3DObject.h
	../src/Projection/CProjection.cpp
	../src/Projection/CProjection.h
	../src/Scene/CScriptScene.cpp
	../src/Scene/CScriptScene.h

)

target_include_directories(Garnet PRIVATE ../src/Library/DawnLib/include ../src/Library/glm  ../src/Library/tinygltf)
# target_include_directories(Garnet PRIVATE ../src/Library/WebGPU ../src/Library/glm  ../src/Library/tinygltf)

if(EMSCRIPTEN)
	target_link_options(Garnet PRIVATE 
		-sEXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString','malloc','free']
		-sUSE_WEBGPU=1
		-sALLOW_MEMORY_GROWTH
		-sFETCH
	)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()
