cmake_minimum_required(VERSION 3.2.2)

project(VersionControlUI)
include(../CMake/common.cmake)

add_library(VersionControlUI SHARED
	src/precompiled.h
	src/VersionControlUIException.h
	src/versioncontrolui_api.h
	src/VersionControlUIPlugin.h
	src/DiffManager.h
	src/nodes/DiffComparisonPair.h
	src/items/VDiffComparisonPair.h
	src/items/VDiffComparisonPairStyle.h
	src/items/ObjectPathCrumb.h
	src/items/ObjectPathCrumbStyle.h
	src/commands/CDiff.h
	src/commands/CHistory.h
	src/handlers/HObjectPathCrumb.h
	src/VersionControlUIException.cpp
	src/VersionControlUIPlugin.cpp
	src/DiffManager.cpp
	src/nodes/DiffComparisonPair.cpp
	src/items/VDiffComparisonPair.cpp
	src/items/VDiffComparisonPairStyle.cpp
	src/items/ObjectPathCrumb.cpp
	src/items/ObjectPathCrumbStyle.cpp
	src/commands/CDiff.cpp
	src/commands/CHistory.cpp
	src/handlers/HObjectPathCrumb.cpp
	test/SimpleTest.cpp
)
envision_plugin(VersionControlUI InteractionBase)
