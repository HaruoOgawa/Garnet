 
cmake_minimum_required(VERSION 3.22.1)

project(Garnet, CXX)

set(CMAKE_BUILD_TYPE Release)

add_executable(
	Garnet
	../src/Test/TestOther.cpp	../src/Test/TestOther.h	../src/Test/TestSubFolder/TestSubClass.cpp	../src/Test/TestSubFolder/TestSubClass.h	../src/WebApp/WebMain.cpp
)

if(EMSCRIPTEN)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()
