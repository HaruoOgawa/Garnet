call C:\emsdk\emsdk_env.bat
call mkdir obj > nul
call del /s /q obj\*  > nul
call echo [1/47]
call emcc -c ../src/Animation/CAnimationChannel.cpp -o ./obj/0.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [2/47]
call emcc -c ../src/Animation/CAnimationClip.cpp -o ./obj/1.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [3/47]
call emcc -c ../src/Animation/CAnimationSampler.cpp -o ./obj/2.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [4/47]
call emcc -c ../src/Animation/CBoneNameProvider.cpp -o ./obj/3.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [5/47]
call emcc -c ../src/Animation/CJoint.cpp -o ./obj/4.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [6/47]
call emcc -c ../src/Animation/CKeyFrame.cpp -o ./obj/5.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [7/47]
call emcc -c ../src/Animation/CSkin.cpp -o ./obj/6.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [8/47]
call emcc -c ../src/App/CWebAppManager.cpp -o ./obj/7.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [9/47]
call emcc -c ../src/App/ScriptApp/CScriptApp.cpp -o ./obj/8.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [10/47]
call emcc -c ../src/Camera/CCamera.cpp -o ./obj/9.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [11/47]
call emcc -c ../src/Camera/CViewerCamera.cpp -o ./obj/10.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [12/47]
call emcc -c ../src/FBX/CFBXImporter.cpp -o ./obj/11.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [13/47]
call emcc -c ../src/FBX/CFBXMomoryStream.cpp -o ./obj/12.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [14/47]
call emcc -c ../src/FBX/CFBXStream.cpp -o ./obj/13.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [15/47]
call emcc -c ../src/FBX/CSmallFBXImporter.cpp -o ./obj/14.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [16/47]
call emcc -c ../src/GLTF/CGLTFImporter.cpp -o ./obj/15.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [17/47]
call emcc -c ../src/Graphics/CDrawInfo.cpp -o ./obj/16.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [18/47]
call emcc -c ../src/Graphics/CMaterial.cpp -o ./obj/17.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [19/47]
call emcc -c ../src/Graphics/CMaterialFrame.cpp -o ./obj/18.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [20/47]
call emcc -c ../src/Graphics/CMesh.cpp -o ./obj/19.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [21/47]
call emcc -c ../src/Graphics/CPresetPrimitive.cpp -o ./obj/20.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [22/47]
call emcc -c ../src/Graphics/CPrimitive.cpp -o ./obj/21.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [23/47]
call emcc -c ../src/Graphics/CShaderBuffer.cpp -o ./obj/22.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [24/47]
call emcc -c ../src/Graphics/CShaderBufferDescriptor.cpp -o ./obj/23.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [25/47]
call emcc -c ../src/Graphics/CTexture.cpp -o ./obj/24.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [26/47]
call emcc -c ../src/Graphics/CTextureSet.cpp -o ./obj/25.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [27/47]
call emcc -c ../src/GraphicsAPI/CMaterialCreateInfo.cpp -o ./obj/26.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [28/47]
call emcc -c ../src/GraphicsAPI/CRendererCreateInfo.cpp -o ./obj/27.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [29/47]
call emcc -c ../src/GraphicsAPI/WebGPU/CWebGPUAPI.cpp -o ./obj/28.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [30/47]
call emcc -c ../src/GraphicsAPI/WebGPU/CWebGPUGPGPUHandler.cpp -o ./obj/29.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [31/47]
call emcc -c ../src/GraphicsAPI/WebGPU/CWebGPUMaterial.cpp -o ./obj/30.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [32/47]
call emcc -c ../src/GraphicsAPI/WebGPU/CWebGPURenderer.cpp -o ./obj/31.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [33/47]
call emcc -c ../src/GraphicsAPI/WebGPU/CWebGPURenderPass.cpp -o ./obj/32.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [34/47]
call emcc -c ../src/GraphicsAPI/WebGPU/CWebGPUTexture.cpp -o ./obj/33.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [35/47]
call emcc -c ../src/ImageEffect/CBlurEffect.cpp -o ./obj/34.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [36/47]
call emcc -c ../src/Input/CInputState.cpp -o ./obj/35.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [37/47]
call emcc -c ../src/LoadWorker/C3DObjectLoader.cpp -o ./obj/36.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [38/47]
call emcc -c ../src/LoadWorker/CFile.cpp -o ./obj/37.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [39/47]
call emcc -c ../src/LoadWorker/CLoadWorker.cpp -o ./obj/38.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [40/47]
call emcc -c ../src/LoadWorker/CMaterialFrameLoader.cpp -o ./obj/39.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [41/47]
call emcc -c ../src/LoadWorker/CTextureLoader.cpp -o ./obj/40.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [42/47]
call emcc -c ../src/Main/WebMain/WebMain.cpp -o ./obj/41.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [43/47]
call emcc -c ../src/Math/CTransform.cpp -o ./obj/42.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [44/47]
call emcc -c ../src/Object/C3DObject.cpp -o ./obj/43.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [45/47]
call emcc -c ../src/Object/CNode.cpp -o ./obj/44.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [46/47]
call emcc -c ../src/Projection/CProjection.cpp -o ./obj/45.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call echo [47/47]
call emcc -c ../src/Scene/CScriptScene.cpp -o ./obj/46.o -I../src/Library/DawnLib/include -I../src/Library/glm -I../src/Library/tinygltf -I../src/Library/SmallFBX/include -DUSE_WEBGPU -DUSE_TEXTURE_LOADER -DUSE_GLTF -DUSE_VIEWER_CAMERA -DUSE_INPUT_SYSTEM -DUSE_GPGPU -DUSE_FBX -DUSE_ANIMATION -DUSE_SMALL_FBX 
call emcc ./obj/0.o ./obj/1.o ./obj/2.o ./obj/3.o ./obj/4.o ./obj/5.o ./obj/6.o ./obj/7.o ./obj/8.o ./obj/9.o ./obj/10.o ./obj/11.o ./obj/12.o ./obj/13.o ./obj/14.o ./obj/15.o ./obj/16.o ./obj/17.o ./obj/18.o ./obj/19.o ./obj/20.o ./obj/21.o ./obj/22.o ./obj/23.o ./obj/24.o ./obj/25.o ./obj/26.o ./obj/27.o ./obj/28.o ./obj/29.o ./obj/30.o ./obj/31.o ./obj/32.o ./obj/33.o ./obj/34.o ./obj/35.o ./obj/36.o ./obj/37.o ./obj/38.o ./obj/39.o ./obj/40.o ./obj/41.o ./obj/42.o ./obj/43.o ./obj/44.o ./obj/45.o ./obj/46.o -s EXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString'] -s USE_WEBGPU=1 -s ALLOW_MEMORY_GROWTH -s FETCH -o Garnet.js
call mkdir ..\WebRelease
 > nulcall del /s /q ..\WebRelease\*
 > nulcall xcopy Garnet_front.js ..\WebRelease\ /y /s /i
call xcopy Garnet*.* ..\WebRelease\ /y /s /i > nul
call xcopy ..\Resources ..\WebRelease\Resources /y /s /i > nul
call xcopy index.html ..\WebRelease\ /y /s /i > nul
pause
