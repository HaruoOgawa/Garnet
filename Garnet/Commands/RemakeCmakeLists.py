import os

Shared_Pre = """ 
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
	-DUSE_FBX
)

add_executable(
	Garnet
"""

# EXPORTED_RUNTIME_METHODSを下記のような感じで,の後ろにスペースを入れるとうまくModuleに登録してくれないみたいなので注意
# EXPORTED_RUNTIME_METHODS=['ccall', 'UTF8ToString']
#									↑ これ
Shared_Post = """
)

target_include_directories(Garnet PRIVATE ../src/Library/DawnLib/include ../src/Library/glm  ../src/Library/tinygltf  ../src/Library/FBX-SDK/include)
# target_include_directories(Garnet PRIVATE ../src/Library/WebGPU ../src/Library/glm  ../src/Library/tinygltf  ../src/Library/FBX-SDK/include)

target_link_libraries(Garnet PRIVATE E:/CppDev/Garnet/Garnet/Src/Library/FBX-SDK/lib/vs2022/x64/release/libfbxsdk-md.lib E:/CppDev/Garnet/Garnet/Src/Library/FBX-SDK/lib/vs2022/x64/release/libxml2-md.lib E:/CppDev/Garnet/Garnet/Src/Library/FBX-SDK/lib/vs2022/x64/release/zlib-md.lib)
#target_link_libraries(Garnet PRIVATE ../Src/Library/FBX-SDK/lib/vs2022/x64/release/)

if(EMSCRIPTEN)
	target_link_options(Garnet PRIVATE 
		-sEXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString','malloc','free']
		-sUSE_WEBGPU=1
		-sALLOW_MEMORY_GROWTH
		-sFETCH
	)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()
"""

ExcludedFolderList = ["Library", "Vulkan", "OpenGL", "DescMain", "CDescAppManager.cpp", "CDescAppManager.h", "DemoMain", "CDemoAppManager.cpp", "CDemoAppManager.h"]

#
def Remake():
	print("[START] Remake CMakeLists\n")

	#
	srcPath = "../src"
	exportText = ""

	#
	pathList = []
	FindDir(os.listdir(srcPath), pathList, srcPath)
	
	#
	exportText += Shared_Pre

	for path in pathList:
		print("[RESULT] %s" %(path))
		exportText += "	" + path + "\n"

	exportText += Shared_Post

	#
	#print(exportText)
	with open("../EmscriptenBuild/CMakeLists.txt", "w") as f:
		f.write(exportText)

# 
def FindDir(subfolders, pathList, parentPath):
	for folder in subfolders:
		currentPath = parentPath + "/" + folder

		if(folder in ExcludedFolderList):
			print("[Exclude] %s" %(folder))
			continue

		if(os.path.isdir(currentPath)):
			FindDir(os.listdir(currentPath), pathList, currentPath)
		else:
			pathList.append(currentPath)

#
Remake()
