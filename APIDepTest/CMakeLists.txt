cmake_minimum_required(VERSION 3.2.2)

project(APIDepTest)
include(../CMake/common.cmake)

add_library(APIDepTest SHARED
	src/precompiled.h
	src/APIDepTestPlugin.h
	src/APIDepTestPlugin.cpp
)
target_link_libraries(APIDepTest HelloWorld)

envision_plugin(APIDepTest)