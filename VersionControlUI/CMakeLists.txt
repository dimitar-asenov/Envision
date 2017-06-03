cmake_minimum_required(VERSION 3.7.2)

project(VersionControlUI)
include(../CMake/common.cmake)

add_library(VersionControlUI SHARED
	src/precompiled.h
	src/VersionControlUIException.h
	src/versioncontrolui_api.h
	src/VersionControlUIPlugin.h
	src/DiffManager.h
	src/nodes/DiffFrame.h
	src/items/VDiffFrame.h
	src/items/VDiffFrameStyle.h
	src/items/ObjectPathCrumb.h
	src/items/ObjectPathCrumbStyle.h
	src/commands/CClear.h
	src/commands/CDiff.h
	src/commands/CHistory.h
	src/handlers/HObjectPathCrumb.h
	src/VersionControlUIException.cpp
	src/VersionControlUIPlugin.cpp
	src/DiffManager.cpp
	src/nodes/DiffFrame.cpp
	src/items/VDiffFrame.cpp
	src/items/VDiffFrameStyle.cpp
	src/items/ObjectPathCrumb.cpp
	src/items/ObjectPathCrumbStyle.cpp
	src/commands/CClear.cpp
	src/commands/CDiff.cpp
	src/commands/CHistory.cpp
	src/handlers/HObjectPathCrumb.cpp
	test/SimpleTest.cpp
)
envision_plugin(VersionControlUI InteractionBase)
