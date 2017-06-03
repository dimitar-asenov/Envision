cmake_minimum_required(VERSION 3.7.2)

project(Core)
include(../CMake/common.cmake)
add_definitions(-DQT_PLUGIN)

find_package(Qt5 REQUIRED Xml)

add_library( Core SHARED
    src/reflect/TypeRegistry.h
    src/Profiler.h
    src/reflect/typeIdMacros.h
    src/reflect/Reflect.h
    src/reflect/Reflect.cpp
    src/AdapterManager.h
    src/core_api.h
    src/core.h
    src/EnvisionApplication.h
    src/EnvisionException.h
    src/EnvisionManager.h
    src/EnvisionPlugin.h
    src/PluginInfo.h
    src/global.h
    src/precompiled.h
    src/TestEvent.h
    src/TestRunner.h
    src/PluginManager.h
    src/EnvisionWindow.h
    src/reflect/TypeRegistry.cpp
    src/Profiler.cpp
    src/AdapterManager.cpp
    src/EnvisionException.cpp
    src/EnvisionManager.cpp
    src/core.cpp
    src/EnvisionApplication.cpp
    src/TestEvent.cpp
    src/TestRunner.cpp
    src/PluginManager.cpp
    src/EnvisionWindow.cpp
    src/global.cpp )

target_link_libraries(Core Qt5::Core Qt5::Widgets Qt5::Xml Qt5::Gui ${COMMON_LIBS})
use_precompiled_header(Core)

install (TARGETS Core DESTINATION ${BUILD_DIR})
