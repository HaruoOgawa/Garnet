import os

Shared_Pre = """ 
cmake_minimum_required(VERSION 3.22.1)

project(Garnet, CXX)

set(CMAKE_BUILD_TYPE Release)

add_definitions(-D__DAWN__)

add_executable(
	Garnet
"""

Shared_Post = """
)

target_include_directories(Garnet PRIVATE ../src/Library/WebGPU)

if(EMSCRIPTEN)
	target_link_options(Garnet PRIVATE 
		-sEXPORTED_RUNTIME_METHODS=['ccall']
		-sUSE_WEBGPU
	)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()
"""

ExcludedFolderList = ["Library", "DescMain", "CDescAppManager.cpp", "CDescAppManager.h","CVulkanAPI.cpp", "CVulkanAPI.h", "CVulkanRenderer.cpp", "CVulkanRenderer.h"]

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
