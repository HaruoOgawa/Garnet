 
cmake_minimum_required(VERSION 3.22.1)

# declare using C and C++
project(Garnet, C CXX)

set(CMAKE_BUILD_TYPE Release)

add_definitions(
	-D__CMAKE__
	-DUSE_WEBGPU
	-DUSE_WebGPUNative
	-DUSE_TEXTURE_LOADER
	-DUSE_GLTF
	-DUSE_VIEWER_CAMERA
	-DUSE_INPUT_SYSTEM
	-DUSE_GPGPU
	-DUSE_FBX
	-DUSE_ANIMATION
	-DUSE_SMALL_FBX
)

add_executable(
	Garnet
	../src/Animation/CAnimationChannel.cpp
	../src/Animation/CAnimationChannel.h
	../src/Animation/CAnimationClip.cpp
	../src/Animation/CAnimationClip.h
	../src/Animation/CAnimationSampler.cpp
	../src/Animation/CAnimationSampler.h
	../src/Animation/CBoneNameProvider.cpp
	../src/Animation/CBoneNameProvider.h
	../src/Animation/CJoint.cpp
	../src/Animation/CJoint.h
	../src/Animation/CKeyFrame.cpp
	../src/Animation/CKeyFrame.h
	../src/Animation/CSkin.cpp
	../src/Animation/CSkin.h
	../src/Animation/EAnimationTarget.h
	../src/Animation/EHumanoidBones.h
	../src/App/CDemoAppManager.cpp
	../src/App/CDemoAppManager.h
	../src/App/CDescAppManager.cpp
	../src/App/CDescAppManager.h
	../src/App/EAppType.h
	../src/App/ScriptApp/CScriptApp.cpp
	../src/App/ScriptApp/CScriptApp.h
	../src/Camera/CCamera.cpp
	../src/Camera/CCamera.h
	../src/Camera/CViewerCamera.cpp
	../src/Camera/CViewerCamera.h
	../src/Debug/Message/Console.h
	../src/FBX/CFBXImporter.cpp
	../src/FBX/CFBXImporter.h
	../src/FBX/CFBXMomoryStream.cpp
	../src/FBX/CFBXMomoryStream.h
	../src/FBX/CFBXStream.cpp
	../src/FBX/CFBXStream.h
	../src/FBX/CSmallFBXImporter.cpp
	../src/FBX/CSmallFBXImporter.h
	../src/GLTF/CGLTFImporter.cpp
	../src/GLTF/CGLTFImporter.h
	../src/Graphics/CDrawInfo.cpp
	../src/Graphics/CDrawInfo.h
	../src/Graphics/CMaterial.cpp
	../src/Graphics/CMaterial.h
	../src/Graphics/CMaterialFrame.cpp
	../src/Graphics/CMaterialFrame.h
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
	../src/Graphics/CTextureSet.cpp
	../src/Graphics/CTextureSet.h
	../src/Graphics/EBlendType.h
	../src/Graphics/EBufferUpdateTypeh.h
	../src/Graphics/ECullMode.h
	../src/Graphics/ETextureType.h
	../src/Graphics/ETextureUsage.h
	../src/Graphics/SBindingLayout.h
	../src/Graphics/SBufferValueLayout.h
	../src/Graphics/STextureBindingLayout.h
	../src/GraphicsAPI/CMaterialCreateInfo.cpp
	../src/GraphicsAPI/CMaterialCreateInfo.h
	../src/GraphicsAPI/CRendererCreateInfo.cpp
	../src/GraphicsAPI/CRendererCreateInfo.h
	../src/GraphicsAPI/ERenderPassFormat.h
	../src/GraphicsAPI/OpenGL/COpenGLAPI.cpp
	../src/GraphicsAPI/OpenGL/COpenGLAPI.h
	../src/GraphicsAPI/OpenGL/COpenGLGPGPUHandler.cpp
	../src/GraphicsAPI/OpenGL/COpenGLGPGPUHandler.h
	../src/GraphicsAPI/OpenGL/COpenGLMaterial.cpp
	../src/GraphicsAPI/OpenGL/COpenGLMaterial.h
	../src/GraphicsAPI/OpenGL/COpenGLRenderer.cpp
	../src/GraphicsAPI/OpenGL/COpenGLRenderer.h
	../src/GraphicsAPI/OpenGL/COpenGLRenderPass.cpp
	../src/GraphicsAPI/OpenGL/COpenGLRenderPass.h
	../src/GraphicsAPI/OpenGL/COpenGLTexture.cpp
	../src/GraphicsAPI/OpenGL/COpenGLTexture.h
	../src/GraphicsAPI/OpenGL/glDef.h
	../src/GraphicsAPI/OpenGL/wglDef.h
	../src/GraphicsAPI/Vulkan/CVulkanGPGPUHandler.cpp
	../src/GraphicsAPI/Vulkan/CVulkanGPGPUHandler.h
	../src/GraphicsAPI/Vulkan/CVulkanMaterial.cpp
	../src/GraphicsAPI/Vulkan/CVulkanMaterial.h
	../src/GraphicsAPI/Vulkan/CVulkanRenderPass.cpp
	../src/GraphicsAPI/Vulkan/CVulkanRenderPass.h
	../src/GraphicsAPI/Vulkan/CVulkanTexture.cpp
	../src/GraphicsAPI/Vulkan/CVulkanTexture.h
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
	../src/LoadWorker/C3DObjectLoader.cpp
	../src/LoadWorker/C3DObjectLoader.h
	../src/LoadWorker/CFile.cpp
	../src/LoadWorker/CFile.h
	../src/LoadWorker/CLoadWorker.cpp
	../src/LoadWorker/CLoadWorker.h
	../src/LoadWorker/CMaterialFrameLoader.cpp
	../src/LoadWorker/CMaterialFrameLoader.h
	../src/LoadWorker/CTextureLoader.cpp
	../src/LoadWorker/CTextureLoader.h
	../src/LoadWorker/ELoadStatus.h
	../src/Main/DemoMain/DemoMain.cpp
	../src/Main/DescMain/DescMain.cpp
	../src/Math/CMath.h
	../src/Math/CTransform.cpp
	../src/Math/CTransform.h
	../src/Object/C3DObject.cpp
	../src/Object/C3DObject.h
	../src/Object/CNode.cpp
	../src/Object/CNode.h
	../src/Object/E3DObjectType.h
	../src/Projection/CProjection.cpp
	../src/Projection/CProjection.h
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

	target_include_directories(Garnet PRIVATE ../src/Library/GLFW/include ../src/Library/WebGPU ../src/Library/glfw3webgpu ../src/Library/glm  ../src/Library/tinygltf  ../src/Library/SmallFBX/include)
	target_link_directories(Garnet PRIVATE ../src/Library/GLFW/lib ../src/Library/WebGPU/windows-x86_64 E:/CppDev/Garnet/Garnet/Src/Library/SmallFBX/lib/debug)
	target_link_libraries(Garnet glfw3.lib wgpu_native.lib SmallFBX.lib zlib.lib)
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
