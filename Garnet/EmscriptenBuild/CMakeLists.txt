 
cmake_minimum_required(VERSION 3.22.1)

project(Garnet, CXX)

set(CMAKE_BUILD_TYPE Release)

add_executable(
	Garnet
	../src/App/CWebAppManager.cpp
	../src/App/CWebAppManager.h
	../src/App/EAppType.h
	../src/App/EditorApp/CEditorApp.cpp
	../src/App/EditorApp/CEditorApp.h
	../src/App/MainApp/CMainApp.cpp
	../src/App/MainApp/CMainApp.h
	../src/App/ScriptApp/CScriptApp.cpp
	../src/App/ScriptApp/CScriptApp.h
	../src/Debug/Message/Console.h
	../src/GraphicsAPI/CWebGPUAPI.cpp
	../src/GraphicsAPI/CWebGPUAPI.h
	../src/Interface/IApp.h
	../src/Interface/IGraphicsAPI.h
	../src/Main/WebMain/WebMain.cpp

)

if(EMSCRIPTEN)
	target_link_options(Garnet PRIVATE -sEXPORTED_RUNTIME_METHODS=['ccall'])
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()
