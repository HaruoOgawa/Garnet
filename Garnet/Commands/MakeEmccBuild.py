from math import e
import os


ExcludedFolderList = ["Library", "Vulkan", "OpenGL", "DescMain", "CDescAppManager.cpp", "CDescAppManager.h", "DemoMain", "CDemoAppManager.cpp", "CDemoAppManager.h"]
IncludeDirectoryList = ["../src/Library/DawnLib/include", "../src/Library/glm", "../src/Library/tinygltf", "../src/Library/SmallFBX/include"]
PreprocessorList = ["USE_WEBGPU", "USE_TEXTURE_LOADER", "USE_GLTF", "USE_VIEWER_CAMERA", "USE_INPUT_SYSTEM", "USE_GPGPU", "USE_FBX", "USE_ANIMATION", "USE_SMALL_FBX"]

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
			if(currentPath.rfind(".cpp") == -1 and currentPath.rfind(".c") == -1):
				continue
			
			pathList.append(currentPath)

#
def Make():
	print("[START] Remake CMakeLists\n")

	#
	srcPath = "../src"
	exportText = ""

	dstPath = "./obj/"

	#
	pathList = []
	FindDir(os.listdir(srcPath), pathList, srcPath)
	
	# emsdk_env.bat
	exportText += "call C:\\emsdk\\emsdk_env.bat\n"

	# mkdir obj
	exportText += "call mkdir obj > nul\n"
	exportText += "call del /s /q obj\\*  > nul\n"
	
	#
	counter = 0

	for path in pathList:
		print("[RESULT] %s" %(path))
		
	    #
		exportText += "call echo [%d/%d]" % (counter + 1, len(pathList)) + "\n"
		
		#
		exportText += "call emcc -c " + path + " -o " + dstPath + str(counter) + ".o "
		
		# Include Dir
		for inc in IncludeDirectoryList:
			exportText += "-I" + inc + " "

		# Preprocessor
		for pre in PreprocessorList:
			exportText += "-D" + pre + " "

		exportText += "\n"
		
		counter += 1

	# All Link
	exportText += "call emcc "

	for i in range(0, counter):
		exportText += dstPath + str(i) + ".o" + " "

	# Compile Options
	exportText += "-s EXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString'] "
	exportText += "-s USE_WEBGPU=1 "
	exportText += "-s ALLOW_MEMORY_GROWTH "
	exportText += "-s FETCH "

	exportText += "-o Garnet.js\n"

	# Result
	exportText += "call mkdir ..\\WebRelease\n > nul"
	exportText += "call del /s /q ..\\WebRelease\\*\n > nul"
	exportText += "call xcopy Garnet_front.js ..\\WebRelease\\ /y /s /i\n"
	exportText += "call xcopy Garnet*.* ..\\WebRelease\\ /y /s /i > nul\n"
	exportText += "call xcopy ..\\Resources ..\\WebRelease\\Resources /y /s /i > nul\n"
	exportText += "call xcopy index.html ..\\WebRelease\\ /y /s /i > nul\n"
	
	exportText += "pause\n"

	#
	#print(exportText)
	with open("../EmscriptenBuild/build_emcc.bat", "w") as f:
		f.write(exportText)

#
Make()
