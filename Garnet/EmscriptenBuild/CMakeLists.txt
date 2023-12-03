 
cmake_minimum_required(VERSION 3.22.1)

# declare using C and C++
project(Garnet, C CXX)

set(CMAKE_BUILD_TYPE debug)

add_definitions(
	-DUSE_WEBGPU
	-DUSE_TEXTURE_LOADER
	-DUSE_GLTF
	-DUSE_VIEWER_CAMERA
	-DUSE_INPUT_SYSTEM
	-DUSE_GPGPU
	-DUSE_FBX
	-DUSE_ANIMATION
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
	../src/FBX/CFBXImporter.cpp
	../src/FBX/CFBXImporter.h
	../src/FBX/CFBXMomoryStream.cpp
	../src/FBX/CFBXMomoryStream.h
	../src/FBX/CFBXStream.cpp
	../src/FBX/CFBXStream.h
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
	../src/Main/WebMain/WebMain.cpp
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

)

target_include_directories(Garnet PRIVATE ../src/Library/DawnLib/include ../src/Library/glm  ../src/Library/tinygltf  ../src/Library/FBX-SDK/include)
# target_include_directories(Garnet PRIVATE ../src/Library/WebGPU ../src/Library/glm  ../src/Library/tinygltf  ../src/Library/FBX-SDK/include)

target_link_libraries(Garnet PRIVATE E:/CppDev/Garnet/Garnet/Src/Library/FBX-SDK/lib/vs2022/x64/debug/libfbxsdk-md.lib E:/CppDev/Garnet/Garnet/Src/Library/FBX-SDK/lib/vs2022/x64/debug/libxml2-md.lib E:/CppDev/Garnet/Garnet/Src/Library/FBX-SDK/lib/vs2022/x64/debug/zlib-md.lib)
#target_link_libraries(Garnet PRIVATE ../Src/Library/FBX-SDK/lib/vs2022/x64/debug/)

if(EMSCRIPTEN)
	target_link_options(Garnet PRIVATE 
		-sEXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString','malloc','free']
		-sUSE_WEBGPU=1
		-sALLOW_MEMORY_GROWTH
		-sFETCH
	)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
	set(CMAKE_CXX_FLAGS_RELEASE "/MD")
endif()
