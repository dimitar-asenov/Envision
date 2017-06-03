cmake_minimum_required(VERSION 3.7.2)

project(Launcher)
include(../CMake/common.cmake)

add_executable( Envision src/launcher.cpp )

target_link_libraries(Envision Core Qt5::Core Qt5::Gui)

install (TARGETS Envision DESTINATION ${BUILD_DIR})
