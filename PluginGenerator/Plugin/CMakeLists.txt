cmake_minimum_required(VERSION 3.7.2)

project(PLUGINNAME)
include(../CMake/common.cmake)

add_library(PLUGINNAME SHARED
	src/precompiled.h
	src/PLUGINNAMEException.h
	src/PLUGINNAME_LOWERCASE_api.h
	src/PLUGINNAMEPlugin.h
	src/PLUGINNAMEException.cpp
	src/PLUGINNAMEPlugin.cpp
	test/SimpleTest.cpp
)
envision_plugin(PLUGINNAME ModelBase)
