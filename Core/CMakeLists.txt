cmake_minimum_required(VERSION 3.2.2)

project(core)
include(../CMake/common.cmake)

add_definitions(-DCORE_LIBRARY)

#add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

find_package(Qt5Xml)

add_library( core SHARED
    src/reflect/TypeRegistry.h
    src/Profiler.h
    src/reflect/typeIdMacros.h
    src/reflect/Reflect.h
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
add_dependencies(core precompiled)

install (TARGETS core DESTINATION ${BUILD_DIR})

target_link_libraries(core Qt5::Core Qt5::Widgets Qt5::Xml Qt5::Gui ${COMMON_LIBS})
