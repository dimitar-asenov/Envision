cmake_minimum_required(VERSION 3.7.2)

project(Logger)
include(../CMake/common.cmake)

add_library(Logger SHARED
	src/Timer.h
	src/Log.h
	src/logger_api.h
	src/precompiled.h
	test/LogTester.h
	src/DefaultLog.h
	src/LoggerPlugin.h
	src/Timer.cpp
	test/LogTester.cpp
	src/DefaultLog.cpp
	src/Log.cpp
	src/LoggerPlugin.cpp
)

envision_plugin(Logger SelfTest)