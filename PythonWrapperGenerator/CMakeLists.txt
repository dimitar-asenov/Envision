cmake_minimum_required(VERSION 3.7.2)

project(PythonWrapperGenerator)
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
set(clangLibs_ -lclangTooling -lclangFrontendTool -lclangFrontend -lclangDriver -lclangSerialization -lclangParse -lclangSema -lclangAnalysis -lclangEdit -lclangAST -lclangLex -lclangBasic ${clangLibs_})

execute_process(COMMAND ${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --system-libs
	OUTPUT_VARIABLE clangDependencies_)
string(STRIP "${clangDependencies_}" clangDependencies_)

add_definitions(-DPythonWrapperGenerator_EXPORTS -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

add_executable(PythonWrapperGenerator
	src/main.cpp
	src/GenTool.cpp
	src/EnvisionAstConsumer.cpp
	src/GeneratorAction.cpp
	src/EnvisionPPCallbacks.cpp
	src/TypeUtilities.cpp
	src/APIData.cpp
	src/APIPrinter.cpp
	src/Config.cpp
	src/GenTool.h
	src/EnvisionAstConsumer.h
	src/GeneratorAction.h
	src/EnvisionPPCallbacks.h
	src/TypeUtilities.h
	src/APIData.h
	src/APIPrinter.h
	src/Config.h
	src/precompiled.h
)

target_link_libraries(PythonWrapperGenerator Qt5::Core ${clangLibs_} ${clangDependencies_})
set_target_properties(PythonWrapperGenerator PROPERTIES AUTOMOC OFF)
use_precompiled_header(PythonWrapperGenerator)

install(FILES "config.json" DESTINATION ${BUILD_DIR}/tools)
install (TARGETS PythonWrapperGenerator DESTINATION ${BUILD_DIR}/tools)
