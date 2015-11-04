cmake_minimum_required(VERSION 3.2.2)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)


set(ENVISION_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)

set(BUILD_DIR ${ENVISION_ROOT_DIR}/DebugBuild)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_DIR ${ENVISION_ROOT_DIR}/DebugBuild)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_DIR ${ENVISION_ROOT_DIR}/ReleaseBuild)
endif()

set(PLUGINS_DIR ${BUILD_DIR}/plugins)

# ???
# CONFIG(debug, debug|release):DEFINES += DEBUG

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -pedantic-errors -Werror -Wextra -O2 -g -fno-omit-frame-pointer -Woverloaded-virtual")

# ???
# clang:QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN'") # Used so that the main executable can link link to core
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN/qt'") # Used so that the main executable can link to custom built Qt
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN/..'") # Used so that plugins can link to core
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN/../qt'") # Used so that plugins can link to custom built Qt

if( WIN32 )
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--export-all-symbols'") #Export all symbols in Windows to save a hassle
endif()

set(COMMON_LIBS "")
if( UNIX )
    set(COMMON_LIBS "-lprofiler")
endif()

# ??? Do we still need this?
# INCLUDEPATH += ./src ./test $${ENVISION_ROOT_DIR}

# ???
# CONFIG += qt warn_on thread precompile_header debug_and_release depend_includepath

# ???
# PRECOMPILED_HEADER = src/precompiled.h

# ???
# QMAKE_POST_LINK += $${ENVISION_ROOT_DIR}/checkers/vera++-check-dir $$_PRO_FILE_PWD_
