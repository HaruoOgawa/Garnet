call C:\emsdk\emsdk_env.bat
call mkdir obj > nul
call del /s /q obj\*  > nul
call echo [1/59] ../src/Animation/CAnimationChannel.cpp
call emcc -o2 -c ../src/Animation/CAnimationChannel.cpp -o ./obj/0.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [2/59] ../src/Animation/CAnimationClip.cpp
call emcc -o2 -c ../src/Animation/CAnimationClip.cpp -o ./obj/1.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [3/59] ../src/Animation/CAnimationController.cpp
call emcc -o2 -c ../src/Animation/CAnimationController.cpp -o ./obj/2.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [4/59] ../src/Animation/CAnimationSampler.cpp
call emcc -o2 -c ../src/Animation/CAnimationSampler.cpp -o ./obj/3.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [5/59] ../src/Animation/CBoneNameProvider.cpp
call emcc -o2 -c ../src/Animation/CBoneNameProvider.cpp -o ./obj/4.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [6/59] ../src/Animation/CJoint.cpp
call emcc -o2 -c ../src/Animation/CJoint.cpp -o ./obj/5.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [7/59] ../src/Animation/CKeyFrame.cpp
call emcc -o2 -c ../src/Animation/CKeyFrame.cpp -o ./obj/6.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [8/59] ../src/Animation/CSkin.cpp
call emcc -o2 -c ../src/Animation/CSkin.cpp -o ./obj/7.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [9/59] ../src/App/CWebAppManager.cpp
call emcc -o2 -c ../src/App/CWebAppManager.cpp -o ./obj/8.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [10/59] ../src/App/ScriptApp/CScriptApp.cpp
call emcc -o2 -c ../src/App/ScriptApp/CScriptApp.cpp -o ./obj/9.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [11/59] ../src/Camera/CCamera.cpp
call emcc -o2 -c ../src/Camera/CCamera.cpp -o ./obj/10.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [12/59] ../src/Camera/CViewerCamera.cpp
call emcc -o2 -c ../src/Camera/CViewerCamera.cpp -o ./obj/11.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [13/59] ../src/FBX/CFBXImporter.cpp
call emcc -o2 -c ../src/FBX/CFBXImporter.cpp -o ./obj/12.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [14/59] ../src/FBX/CFBXMomoryStream.cpp
call emcc -o2 -c ../src/FBX/CFBXMomoryStream.cpp -o ./obj/13.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [15/59] ../src/FBX/CFBXStream.cpp
call emcc -o2 -c ../src/FBX/CFBXStream.cpp -o ./obj/14.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [16/59] ../src/FBX/CSmallFBXImporter.cpp
call emcc -o2 -c ../src/FBX/CSmallFBXImporter.cpp -o ./obj/15.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [17/59] ../src/GLTF/CGLTFImporter.cpp
call emcc -o2 -c ../src/GLTF/CGLTFImporter.cpp -o ./obj/16.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [18/59] ../src/Graphics/CDrawInfo.cpp
call emcc -o2 -c ../src/Graphics/CDrawInfo.cpp -o ./obj/17.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [19/59] ../src/Graphics/CMaterial.cpp
call emcc -o2 -c ../src/Graphics/CMaterial.cpp -o ./obj/18.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [20/59] ../src/Graphics/CMaterialFrame.cpp
call emcc -o2 -c ../src/Graphics/CMaterialFrame.cpp -o ./obj/19.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [21/59] ../src/Graphics/CMesh.cpp
call emcc -o2 -c ../src/Graphics/CMesh.cpp -o ./obj/20.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [22/59] ../src/Graphics/CPresetPrimitive.cpp
call emcc -o2 -c ../src/Graphics/CPresetPrimitive.cpp -o ./obj/21.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [23/59] ../src/Graphics/CPrimitive.cpp
call emcc -o2 -c ../src/Graphics/CPrimitive.cpp -o ./obj/22.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [24/59] ../src/Graphics/CShaderBuffer.cpp
call emcc -o2 -c ../src/Graphics/CShaderBuffer.cpp -o ./obj/23.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [25/59] ../src/Graphics/CShaderBufferDescriptor.cpp
call emcc -o2 -c ../src/Graphics/CShaderBufferDescriptor.cpp -o ./obj/24.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [26/59] ../src/Graphics/CTexture.cpp
call emcc -o2 -c ../src/Graphics/CTexture.cpp -o ./obj/25.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [27/59] ../src/Graphics/CTextureSet.cpp
call emcc -o2 -c ../src/Graphics/CTextureSet.cpp -o ./obj/26.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [28/59] ../src/GraphicsAPI/CMaterialCreateInfo.cpp
call emcc -o2 -c ../src/GraphicsAPI/CMaterialCreateInfo.cpp -o ./obj/27.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [29/59] ../src/GraphicsAPI/CRendererCreateInfo.cpp
call emcc -o2 -c ../src/GraphicsAPI/CRendererCreateInfo.cpp -o ./obj/28.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [30/59] ../src/GraphicsAPI/WebGPU/CWebGPUAPI.cpp
call emcc -o2 -c ../src/GraphicsAPI/WebGPU/CWebGPUAPI.cpp -o ./obj/29.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [31/59] ../src/GraphicsAPI/WebGPU/CWebGPUGPGPUHandler.cpp
call emcc -o2 -c ../src/GraphicsAPI/WebGPU/CWebGPUGPGPUHandler.cpp -o ./obj/30.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [32/59] ../src/GraphicsAPI/WebGPU/CWebGPUMaterial.cpp
call emcc -o2 -c ../src/GraphicsAPI/WebGPU/CWebGPUMaterial.cpp -o ./obj/31.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [33/59] ../src/GraphicsAPI/WebGPU/CWebGPURenderer.cpp
call emcc -o2 -c ../src/GraphicsAPI/WebGPU/CWebGPURenderer.cpp -o ./obj/32.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [34/59] ../src/GraphicsAPI/WebGPU/CWebGPURenderPass.cpp
call emcc -o2 -c ../src/GraphicsAPI/WebGPU/CWebGPURenderPass.cpp -o ./obj/33.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [35/59] ../src/GraphicsAPI/WebGPU/CWebGPUTexture.cpp
call emcc -o2 -c ../src/GraphicsAPI/WebGPU/CWebGPUTexture.cpp -o ./obj/34.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [36/59] ../src/ImageEffect/CBlurEffect.cpp
call emcc -o2 -c ../src/ImageEffect/CBlurEffect.cpp -o ./obj/35.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [37/59] ../src/Input/CInputState.cpp
call emcc -o2 -c ../src/Input/CInputState.cpp -o ./obj/36.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [38/59] ../src/LoadWorker/C3DObjectLoader.cpp
call emcc -o2 -c ../src/LoadWorker/C3DObjectLoader.cpp -o ./obj/37.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [39/59] ../src/LoadWorker/CFile.cpp
call emcc -o2 -c ../src/LoadWorker/CFile.cpp -o ./obj/38.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [40/59] ../src/LoadWorker/CLoadWorker.cpp
call emcc -o2 -c ../src/LoadWorker/CLoadWorker.cpp -o ./obj/39.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [41/59] ../src/LoadWorker/CMaterialFrameLoader.cpp
call emcc -o2 -c ../src/LoadWorker/CMaterialFrameLoader.cpp -o ./obj/40.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [42/59] ../src/LoadWorker/CTextureLoader.cpp
call emcc -o2 -c ../src/LoadWorker/CTextureLoader.cpp -o ./obj/41.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [43/59] ../src/Main/WebMain/WebMain.cpp
call emcc -o2 -c ../src/Main/WebMain/WebMain.cpp -o ./obj/42.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [44/59] ../src/Math/CTransform.cpp
call emcc -o2 -c ../src/Math/CTransform.cpp -o ./obj/43.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [45/59] ../src/Object/C3DObject.cpp
call emcc -o2 -c ../src/Object/C3DObject.cpp -o ./obj/44.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [46/59] ../src/Object/CNode.cpp
call emcc -o2 -c ../src/Object/CNode.cpp -o ./obj/45.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [47/59] ../src/Projection/CProjection.cpp
call emcc -o2 -c ../src/Projection/CProjection.cpp -o ./obj/46.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [48/59] ../src/Scene/CScriptScene.cpp
call emcc -o2 -c ../src/Scene/CScriptScene.cpp -o ./obj/47.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [49/59] ../Src/Library/SmallFBX/include/SmallFBX/pch.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/pch.cpp -o ./obj/48.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [50/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxAnimation.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxAnimation.cpp -o ./obj/49.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [51/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxDeformer.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxDeformer.cpp -o ./obj/50.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [52/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxDocument.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxDocument.cpp -o ./obj/51.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [53/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxGeometry.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxGeometry.cpp -o ./obj/52.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [54/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxMaterial.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxMaterial.cpp -o ./obj/53.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [55/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxModel.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxModel.cpp -o ./obj/54.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [56/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxNode.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxNode.cpp -o ./obj/55.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [57/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxObject.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxObject.cpp -o ./obj/56.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [58/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxProperty.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxProperty.cpp -o ./obj/57.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [59/59] ../Src/Library/SmallFBX/include/SmallFBX/sfbxUtils.cpp
call emcc -o2 -c ../Src/Library/SmallFBX/include/SmallFBX/sfbxUtils.cpp -o ./obj/58.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call emcc -o2 ./obj/0.o ./obj/1.o ./obj/2.o ./obj/3.o ./obj/4.o ./obj/5.o ./obj/6.o ./obj/7.o ./obj/8.o ./obj/9.o ./obj/10.o ./obj/11.o ./obj/12.o ./obj/13.o ./obj/14.o ./obj/15.o ./obj/16.o ./obj/17.o ./obj/18.o ./obj/19.o ./obj/20.o ./obj/21.o ./obj/22.o ./obj/23.o ./obj/24.o ./obj/25.o ./obj/26.o ./obj/27.o ./obj/28.o ./obj/29.o ./obj/30.o ./obj/31.o ./obj/32.o ./obj/33.o ./obj/34.o ./obj/35.o ./obj/36.o ./obj/37.o ./obj/38.o ./obj/39.o ./obj/40.o ./obj/41.o ./obj/42.o ./obj/43.o ./obj/44.o ./obj/45.o ./obj/46.o ./obj/47.o ./obj/48.o ./obj/49.o ./obj/50.o ./obj/51.o ./obj/52.o ./obj/53.o ./obj/54.o ./obj/55.o ./obj/56.o ./obj/57.o ./obj/58.o ../EmscriptenBuild/obj_lib/zlib/adler32.o ../EmscriptenBuild/obj_lib/zlib/compress.o ../EmscriptenBuild/obj_lib/zlib/crc32.o ../EmscriptenBuild/obj_lib/zlib/deflate.o ../EmscriptenBuild/obj_lib/zlib/gzclose.o ../EmscriptenBuild/obj_lib/zlib/gzlib.o ../EmscriptenBuild/obj_lib/zlib/gzread.o ../EmscriptenBuild/obj_lib/zlib/gzwrite.o ../EmscriptenBuild/obj_lib/zlib/infback.o ../EmscriptenBuild/obj_lib/zlib/inffast.o ../EmscriptenBuild/obj_lib/zlib/inflate.o ../EmscriptenBuild/obj_lib/zlib/inftrees.o ../EmscriptenBuild/obj_lib/zlib/trees.o ../EmscriptenBuild/obj_lib/zlib/uncompr.o ../EmscriptenBuild/obj_lib/zlib/zutil.o -s EXPORTED_RUNTIME_METHODS='ccall','UTF8ToString' -s USE_WEBGPU=1 -s ALLOW_MEMORY_GROWTH -s FETCH -o Garnet.js
call mkdir ..\WebRelease
 > nulcall del /s /q ..\WebRelease\*
 > nulcall xcopy Garnet_front.js ..\WebRelease\ /y /s /i
call xcopy Garnet*.* ..\WebRelease\ /y /s /i
call xcopy ..\Resources ..\WebRelease\Resources /y /s /i > nul
call xcopy index.html ..\WebRelease\ /y /s /i > nul
pause
