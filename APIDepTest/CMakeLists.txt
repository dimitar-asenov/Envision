cmake_minimum_required(VERSION 3.7.2)

project(APIDepTest)
include(../CMake/common.cmake)

add_library(APIDepTest SHARED
	src/precompiled.h
	src/apideptest_api.h
	src/APIDepTestPlugin.h
	src/APIDepTestPlugin.cpp
)
envision_plugin(APIDepTest HelloWorld)