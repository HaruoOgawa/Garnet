 
cmake_minimum_required(VERSION 3.22.1)

project(Garnet, CXX)

set(CMAKE_BUILD_TYPE Release)

add_executable(
	Garnet
	../src/App/CApp.cpp
	../src/App/CApp.h
	../src/App/MainApp/CMainApp.cpp
	../src/App/MainApp/CMainApp.h
	../src/Debug/Message/Console.h
	../src/Main/WebMain/WebMain.cpp

)

if(EMSCRIPTEN)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()
