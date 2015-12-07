cmake_minimum_required(VERSION 3.2.2)

project(AlloyIntegration)
include(../CMake/common.cmake)

add_library(AlloyIntegration SHARED
	src/precompiled.h
	src/AlloyIntegrationException.h
	src/alloyintegration_api.h
	src/AlloyIntegrationPlugin.h
	src/commands/CAlloy.h
	src/visitors/AlloyVisitor.h
	src/exporter/AlloyExporter.h
	src/AlloyIntegrationException.cpp
	src/AlloyIntegrationPlugin.cpp
	test/SimpleTest.cpp
	src/commands/CAlloy.cpp
	src/visitors/AlloyVisitor.cpp
	src/exporter/AlloyExporter.cpp
)

envision_plugin(AlloyIntegration ContractsLibrary)
install(DIRECTORY alloy_data/ DESTINATION ${BUILD_DIR}/alloy)