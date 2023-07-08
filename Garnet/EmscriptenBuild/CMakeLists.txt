 
cmake_minimum_required(VERSION 3.22.1)

# declare using C and C++
project(Garnet, C CXX)

set(CMAKE_BUILD_TYPE Release)

add_definitions(
	-D__DAWN__
	-DUSE_TEXTURE_LOADER
	-DUSE_GLTF
	-DUSE_VIEWER_CAMERA
	-DUSE_INPUT_SYSTEM
)

add_executable(
	Garnet
	../src/App/CDemoAppManager.cpp
	../src/App/CDemoAppManager.h
	../src/App/CWebAppManager.cpp
	../src/App/CWebAppManager.h
	../src/App/EAppType.h
	../src/App/EditorApp/CEditorApp.cpp
	../src/App/EditorApp/CEditorApp.h
	../src/App/MainApp/CMainApp.cpp
	../src/App/MainApp/CMainApp.h
	../src/App/ScriptApp/CScriptApp.cpp
	../src/App/ScriptApp/CScriptApp.h
	../src/Camera/CCamera.cpp
	../src/Camera/CCamera.h
	../src/Camera/CViewerCamera.cpp
	../src/Camera/CViewerCamera.h
	../src/Debug/Message/Console.h
	../src/File/CFileReader.cpp
	../src/File/CFileReader.h
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
	../src/Graphics/CTexture.cpp
	../src/Graphics/CTexture.h
	../src/Graphics/CUniformBuffer.cpp
	../src/Graphics/CUniformBuffer.h
	../src/Graphics/CUniformBufferDescriptor.cpp
	../src/Graphics/CUniformBufferDescriptor.h
	../src/Graphics/ETextureType.h
	../src/Graphics/SBindingLayout.h
	../src/Graphics/STextureBindingLayout.h
	../src/GraphicsAPI/CMaterialCreateInfo.cpp
	../src/GraphicsAPI/CMaterialCreateInfo.h
	../src/GraphicsAPI/CRendererCreateInfo.cpp
	../src/GraphicsAPI/CRendererCreateInfo.h
	../src/GraphicsAPI/ERenderPassFormat.h
	../src/GraphicsAPI/Vulkan/CVulkanMaterial.cpp
	../src/GraphicsAPI/Vulkan/CVulkanMaterial.h
	../src/GraphicsAPI/Vulkan/CVulkanRenderPass.cpp
	../src/GraphicsAPI/Vulkan/CVulkanRenderPass.h
	../src/GraphicsAPI/Vulkan/CVulkanTexture.cpp
	../src/GraphicsAPI/Vulkan/CVulkanTexture.h
	../src/GraphicsAPI/WebGPU/CWebGPUAPI.cpp
	../src/GraphicsAPI/WebGPU/CWebGPUAPI.h
	../src/GraphicsAPI/WebGPU/CWebGPUMaterial.cpp
	../src/GraphicsAPI/WebGPU/CWebGPUMaterial.h
	../src/GraphicsAPI/WebGPU/CWebGPURenderer.cpp
	../src/GraphicsAPI/WebGPU/CWebGPURenderer.h
	../src/GraphicsAPI/WebGPU/CWebGPURenderPass.cpp
	../src/GraphicsAPI/WebGPU/CWebGPURenderPass.h
	../src/GraphicsAPI/WebGPU/CWebGPUTexture.cpp
	../src/GraphicsAPI/WebGPU/CWebGPUTexture.h
	../src/Input/CInputState.cpp
	../src/Input/CInputState.h
	../src/Interface/IApp.h
	../src/Interface/IBuffer.h
	../src/Interface/IDescriptor.h
	../src/Interface/IGraphicsAPI.h
	../src/Interface/IRenderer.h
	../src/Interface/IRenderPass.h
	../src/Main/DemoMain/DemoMain.cpp
	../src/Main/WebMain/WebMain.cpp
	../src/Math/CMath.h
	../src/Math/CTransform.cpp
	../src/Math/CTransform.h
	../src/Object/C3DObject.cpp
	../src/Object/C3DObject.h
	../src/Object/CNode.cpp
	../src/Object/CNode.h
	../src/Object/CObject.cpp
	../src/Object/CObject.h
	../src/Projection/CProjection.cpp
	../src/Projection/CProjection.h
	../src/Scene/CScriptScene.cpp
	../src/Scene/CScriptScene.h

)

target_include_directories(Garnet PRIVATE ../src/Library/WebGPU ../src/Library/glm  ../src/Library/tinygltf)

if(EMSCRIPTEN)
	target_link_options(Garnet PRIVATE 
		-sEXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString','malloc','free']
		-sUSE_WEBGPU=1
		-sALLOW_MEMORY_GROWTH
		-sFETCH
	)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()
