cmake_minimum_required(VERSION 3.2.2)

project(CodeReview)
include(../CMake/common.cmake)

add_library(CodeReview SHARED
	src/precompiled.h
	src/CodeReviewException.h
	src/codereview_api.h
	src/CodeReviewPlugin.h
	src/CodeReviewException.cpp
	src/CodeReviewPlugin.cpp
	test/SimpleTest.cpp
)
envision_plugin(CodeReview OOInteraction)
