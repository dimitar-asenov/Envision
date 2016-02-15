cmake_minimum_required(VERSION 3.2.2)

project(SelfTest)
include(../CMake/common.cmake)

add_library(SelfTest SHARED
	src/Test.h
	src/TestAssertions.h
	src/TestManager.h
	src/TestResult.h
	src/TestResults.h
	src/precompiled.h
	src/selftest_api.h
	src/SelfTestPlugin.h
	src/TestResult.cpp
	src/TestResults.cpp
	src/TestBase.cpp
        src/TestBase.h
	src/SelfTestPlugin.cpp
)

envision_plugin(SelfTest)