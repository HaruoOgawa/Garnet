import os

Shared_Pre = """ 
cmake_minimum_required(VERSION 3.22.1)

project(Garnet, CXX)

set(CMAKE_BUILD_TYPE Release)

add_definitions(-D__DAWN__ -D__CMAKE__)

add_executable(
	Garnet
"""

Shared_Post = """
)

set_target_properties(
	Garnet PROPERTIES
	CXX_STANDARD 17
	COMPILE_WARNING_AS_ERROR ON
	LINK_FLAGS /SUBSYSTEM:CONSOLE
)

target_include_directories(Garnet PRIVATE ../src/Library/GLFW/include ../src/Library/WebGPU)
target_link_directories(Garnet PRIVATE ../src/Library/GLFW/lib ../src/Library/WebGPU/windows-x86_64)
target_link_libraries(Garnet glfw3.lib wgpu_native.lib)

if (MSVC)
	target_compile_options(Garnet PRIVATE /W4)
else()
	target_compile_options(Garnet PRIVATE -Wall -Wextra -pedantic)
endif()
"""

ExcludedFolderList = ["Library", "WebMain", "CWebAppManager.cpp", "CWebAppManager.h","CVulkanAPI.cpp", "CVulkanAPI.h", "CVulkanRenderer.cpp", "CVulkanRenderer.h"]

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
	with open("../CMakeBuild/CMakeLists.txt", "w") as f:
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
