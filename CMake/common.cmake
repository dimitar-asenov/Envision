cmake_minimum_required(VERSION 3.2.2)

# This is specific for using a Make-based build
# find_program(CCACHE_FOUND ccache)
# if(CCACHE_FOUND)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
# endif(CCACHE_FOUND)

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

#add_library(precompiled ${PROJECT_SOURCE_DIR}/src/precompiled.h)
#add_library(precompiled OBJECT src/precompiled.h)
set(precompiled_cpp ${CMAKE_CURRENT_BINARY_DIR}/precompiled_cpp.cpp)
FILE(WRITE ${precompiled_cpp} "#include <precompiled.h>\n")
set_source_files_properties(${precompiled_cpp}  PROPERTIES COMPILE_FLAGS "-include ${CMAKE_CURRENT_SOURCE_DIR}/src/precompiled.h")
add_library(precompiled OBJECT src/precompiled.h ${precompiled_cpp})

get_target_property(_loc Qt5::Core LOCATION)
message("Plugin Qt5::Core is at location ${_loc}")
  
set_source_files_properties(src/precompiled.h PROPERTIES COMPILE_FLAGS "-x c++-header")
set_target_properties(precompiled PROPERTIES AUTOMOC OFF)
set_target_properties(precompiled PROPERTIES OUTPUT_NAME "precompiled.h.gch")
set_target_properties(precompiled PROPERTIES COMPILE_FLAGS "-x c++-header")

# ???
# PRECOMPILED_HEADER = src/precompiled.h

# ???
# QMAKE_POST_LINK += $${ENVISION_ROOT_DIR}/checkers/vera++-check-dir $$_PRO_FILE_PWD_
