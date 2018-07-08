cmake_minimum_required(VERSION 3.7.2)

project(CppImportLight)
include(../CMake/common.cmake)

add_library(CppImportLight SHARED
	src/precompiled.h
	src/CppImportLightException.h
	src/cppimportlight_api.h
	src/CppImportLightPlugin.h
	src/CppImportLightException.cpp
	src/CppImportLightPlugin.cpp
	test/SimpleTest.cpp
)
envision_plugin(CppImportLight OOInteraction)
