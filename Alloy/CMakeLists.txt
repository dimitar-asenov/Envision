cmake_minimum_required(VERSION 3.7.2)

project(Alloy)
include(../CMake/common.cmake)

add_library(Alloy SHARED
	src/precompiled.h
	src/AlloyException.h
	src/alloy_api.h
	src/AlloyPlugin.h
	src/commands/CAlloy.h
	src/visitors/AlloyVisitor.h
	src/exporter/AlloyExporter.h
	src/AlloyException.cpp
	src/AlloyPlugin.cpp
	test/SimpleTest.cpp
	src/commands/CAlloy.cpp
	src/visitors/AlloyVisitor.cpp
	src/exporter/AlloyExporter.cpp
)

envision_plugin(Alloy ContractsLibrary)
install(DIRECTORY alloy_data/ DESTINATION ${BUILD_DIR}/alloy)