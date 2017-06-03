cmake_minimum_required(VERSION 3.7.2)

project(CustomMethodCall)
include(../CMake/common.cmake)

add_library(CustomMethodCall SHARED
	src/CustomMethodCallException.h
	src/custommethodcall_api.h
	src/items/EmptyMethodVis.h
	src/items/EmptyMethodVisStyle.h
	src/items/FindMethodVis.h
	src/items/FindMethodVisStyle.h
	src/items/InsertMethodVis.h
	src/items/InsertMethodVisStyle.h
	src/items/SumMethodVis.h
	src/items/SumMethodVisStyle.h
	src/precompiled.h
	src/CustomMethodCallPlugin.h
	src/items/SumMethodVis.cpp
	src/items/SumMethodVisStyle.cpp
	src/items/InsertMethodVis.cpp
	src/items/InsertMethodVisStyle.cpp
	src/items/EmptyMethodVisStyle.cpp
	src/items/EmptyMethodVis.cpp
	src/items/FindMethodVis.cpp
	src/items/FindMethodVisStyle.cpp
	src/CustomMethodCallException.cpp
	test/SimpleTest.cpp
	src/CustomMethodCallPlugin.cpp
)

envision_plugin(CustomMethodCall OOVisualization)