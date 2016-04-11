cmake_minimum_required(VERSION 3.2.2)

project(VersionControlUI)
include(../CMake/common.cmake)

add_library(VersionControlUI SHARED
	src/precompiled.h
	src/VersionControlUIException.h
	src/versioncontrolui_api.h
	src/VersionControlUIPlugin.h
	src/DiffManager.h
	src/commands/CDiff.h
	src/VersionControlUIException.cpp
	src/VersionControlUIPlugin.cpp
	src/DiffManager.cpp
	src/commands/CDiff.cpp
	test/SimpleTest.cpp
)
envision_plugin(VersionControlUI InteractionBase)
