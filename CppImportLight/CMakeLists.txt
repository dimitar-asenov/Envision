cmake_minimum_required(VERSION 3.7.2)

project(CppImportLight)
include(../CMake/common.cmake)

execute_process(COMMAND ${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --includedir
	OUTPUT_VARIABLE clangIncludes_)
string(STRIP "${clangIncludes_}" clangIncludes_)
include_directories(SYSTEM ${clangIncludes_})

execute_process(COMMAND ${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --ldflags
	OUTPUT_VARIABLE clangLibPath_)
string(STRIP "${clangLibPath_}" clangLibPath_)
string(SUBSTRING "${clangLibPath_}" 2 -1 clangLibPath_) #skip the -L
link_directories(${clangLibPath_})

execute_process(COMMAND ${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --libs
	OUTPUT_VARIABLE clangLibs_)
string(STRIP "${clangLibs_}" clangLibs_)
set(clangLibs_ -lclangLex -lclangBasic ${clangLibs_})

add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

add_library(CppImportLight SHARED
	src/precompiled.h
	src/CppImportLightException.h
	src/cppimportlight_api.h
	src/CppImportLightPlugin.h
	src/CppImportLightException.cpp
	src/CppImportLightPlugin.cpp
	src/Parser.h
	src/Parser.cpp
	test/SimpleTest.cpp
)

target_link_libraries(CppImportLight ${clangLibs_})
envision_plugin(CppImportLight OOInteraction)
