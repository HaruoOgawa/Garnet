 
cmake_minimum_required(VERSION 3.22.1)

# declare using C and C++
project(Garnet, C CXX)

set(CMAKE_BUILD_TYPE debug)

add_definitions(
	-DUSE_WEB_NATIVE
	-DUSE_WEBGPU
	-DNOMINMAX
	-DUSE_TEXTURE_LOADER
	-DUSE_GLTF
	-DUSE_VIEWER_CAMERA
	-DUSE_INPUT_SYSTEM
	-DUSE_GPGPU
	-DUSE_FBX
	-DUSE_ANIMATION
	-DUSE_SMALL_FBX
	-DUSE_MMD
	-DUSE_PHYSICS
	-DB3_USE_CLEW
)

add_executable(
	Garnet
	../src/App/ScriptApp/CScriptApp.cpp
	../src/App/ScriptApp/CScriptApp.h
	../src/App/ScriptApp/Scene/CScriptScene.cpp
	../src/App/ScriptApp/Scene/CScriptScene.h
	../src/Graphics/CFrameRenderer.cpp
	../src/Graphics/CFrameRenderer.h
	../src/GUIApp/GUI/CGraphicsEditingWindow.cpp
	../src/GUIApp/GUI/CGraphicsEditingWindow.h
	../src/GUIApp/GUI/CGUICameraTab.cpp
	../src/GUIApp/GUI/CGUICameraTab.h
	../src/GUIApp/GUI/CGUIMaterialTab.cpp
	../src/GUIApp/GUI/CGUIMaterialTab.h
	../src/GUIApp/GUI/CGUIObjectTab.cpp
	../src/GUIApp/GUI/CGUIObjectTab.h
	../src/GUIApp/GUI/CGUIRenderingTab.cpp
	../src/GUIApp/GUI/CGUIRenderingTab.h
	../src/GUIApp/GUI/CGUIResourcesTab.cpp
	../src/GUIApp/GUI/CGUIResourcesTab.h
	../src/GUIApp/GUI/CGUITransformTab.cpp
	../src/GUIApp/GUI/CGUITransformTab.h
	../src/GUIApp/GUI/CTimeLineView.cpp
	../src/GUIApp/GUI/CTimeLineView.h
	../src/GUIApp/Model/CFileModifier.cpp
	../src/GUIApp/Model/CFileModifier.h
	../src/ImageEffect/CBlurEffect.cpp
	../src/ImageEffect/CBlurEffect.h
	../src/Main/main.cpp

)

message(STATUS "Library directory: ${CMAKE_CURRENT_SOURCE_DIR}/../src/Library/SmallFBX/lib/debug")

target_include_directories(Garnet PRIVATE ../src/Library/DawnLib/include ../src/Library/glm  ../src/Library/tinygltf  ../src/Library/SmallFBX/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/Library/SmallFBX/lib/debug)
target_link_libraries(Garnet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/Library/SmallFBX/lib/debug/libSmallFBX.a ${CMAKE_CURRENT_SOURCE_DIR}/../src/Library/SmallFBX/lib/debug/zlib.a)

# target_link_libraries(Garnet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/Library/SmallFBX/lib/debug/SmallFBX.a ${CMAKE_CURRENT_SOURCE_DIR}/../src/Library/SmallFBX/lib/debug/zlib.a)
# target_link_libraries(Garnet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/Library/SmallFBX/lib/debug/sfbxLibList.a)

set_property(TARGET Garnet PROPERTY CXX_STANDARD 17)

if(EMSCRIPTEN)
	target_link_options(Garnet PRIVATE 
		-sEXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString','malloc','free']
		-sUSE_WEBGPU=1
		-sALLOW_MEMORY_GROWTH
		-sFETCH
	)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()
