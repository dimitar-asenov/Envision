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
	src/handlers/HCodeReviewOverlay.h
	src/nodes/CommentedNode.h
	src/nodes/ReviewComment.h
	src/items/VCommentedNode.h
	src/items/VCommentedNodeStyle.h
	src/items/VReviewComment.h
	src/items/VReviewCommentStyle.h
	src/CodeReviewException.cpp
	src/CodeReviewPlugin.cpp
	src/CodeReviewManager.cpp
	src/commands/CCodeReviewComment.cpp
	src/overlays/CodeReviewCommentOverlay.cpp
	src/overlays/CodeReviewCommentOverlayStyle.cpp
	src/handlers/HReviewableItem.cpp
	src/handlers/HCodeReviewOverlay.cpp
	src/nodes/CommentedNode.cpp
	src/nodes/ReviewComment.cpp
	src/items/VCommentedNode.cpp
	src/items/VCommentedNodeStyle.cpp
	src/items/VReviewComment.cpp
	src/items/VReviewCommentStyle.cpp
	test/SimpleTest.cpp
)
envision_plugin(CodeReview OOInteraction VersionControlUI)
