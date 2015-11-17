cmake_minimum_required(VERSION 3.2.2)

project(CppExport)
include(../CMake/common.cmake)

add_library(CppExport SHARED
	src/precompiled.h
	src/CppExportException.h
	src/cppexport_api.h
	src/CppExportPlugin.h
	src/dependency_analysis/DependencyAnalyzer.h
	src/dependency_analysis/DependencyUnit.h
	src/dependency_analysis/DependencyTarget.h
	src/Config.h
	src/dependency_analysis/DependencyComposite.h
	src/CppExportException.cpp
	src/CppExportPlugin.cpp
	test/SimpleTest.cpp
	src/dependency_analysis/DependencyAnalyzer.cpp
	src/dependency_analysis/DependencyUnit.cpp
	src/Config.cpp
	src/dependency_analysis/DependencyComposite.cpp
)

target_link_libraries(CppExport OOModel Export)
envision_plugin(CppExport)
install(FILES "config.json" DESTINATION ${BUILD_DIR}/cpp-export-settings)