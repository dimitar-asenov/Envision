cmake_minimum_required(VERSION 3.2.2)

project(CodeReview)
include(../CMake/common.cmake)

add_library(CodeReview SHARED
	src/precompiled.h
	src/CodeReviewException.h
	src/codereview_api.h
	src/CodeReviewPlugin.h
	src/CodeReviewManager.h
	src/commands/CCodeReviewComment.h
	src/overlays/CodeReviewCommentOverlay.h
	src/overlays/CodeReviewCommentOverlayStyle.h
	src/handlers/HReviewableItem.h
	src/CodeReviewException.cpp
	src/CodeReviewPlugin.cpp
	src/CodeReviewManager.cpp
	src/commands/CCodeReviewComment.cpp
	src/overlays/CodeReviewCommentOverlay.cpp
	src/overlays/CodeReviewCommentOverlayStyle.cpp
	src/handlers/HReviewableItem.cpp
	test/SimpleTest.cpp
)
envision_plugin(CodeReview OOInteraction VersionControlUI)
