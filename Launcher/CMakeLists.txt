cmake_minimum_required(VERSION 3.2.2)

project(Launcher)
include(../CMake/common.cmake)

add_executable( Envision src/precompiled.h src/launcher.cpp )

target_link_libraries(Envision -lCore Qt5::Core Qt5::Gui)

install (TARGETS Envision DESTINATION ${BUILD_DIR})
