# CMakeのバージョン設定
cmake_minimum_required(VERSION 3.22.1)

# プロジェクト名と言語の設定
project(Garnet, CXX)

# ビルドターゲット
set(CMAKE_BUILD_TYPE Release)

# ビルドするファイルを選択
add_executable(
	Garnet
	../src/WebApp/WebMain.cpp
)

# Emscriptenの設定
if(EMSCRIPTEN)
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()