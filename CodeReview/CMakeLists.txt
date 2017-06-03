cmake_minimum_required(VERSION 3.7.2)

project(CodeReview)
include(../CMake/common.cmake)

add_library(CodeReview SHARED
	src/precompiled.h
	src/CodeReviewException.h
	src/codereview_api.h
	src/CodeReviewPlugin.h
	src/CodeReviewManager.h
	src/commands/CCodeReviewComment.h
	src/commands/CCodeReview.h
	src/commands/CFocus.h
	src/overlays/CodeReviewCommentOverlay.h
	src/overlays/CodeReviewCommentOverlayStyle.h
	src/handlers/HReviewableItem.h
	src/handlers/HCodeReviewOverlay.h
	src/nodes/NodeReviews.h
	src/nodes/NodeReviewsList.h
	src/nodes/ReviewComment.h
	src/items/VNodeReviews.h
	src/items/VNodeReviewsStyle.h
	src/items/VReviewComment.h
	src/items/VReviewCommentStyle.h
	src/orderings/Orderings.h
	src/groupings/UseAnalysisGroupings.h
	src/CodeReviewException.cpp
	src/CodeReviewPlugin.cpp
	src/CodeReviewManager.cpp
	src/commands/CCodeReviewComment.cpp
	src/commands/CCodeReview.cpp
	src/commands/CFocus.cpp
	src/overlays/CodeReviewCommentOverlay.cpp
	src/overlays/CodeReviewCommentOverlayStyle.cpp
	src/handlers/HReviewableItem.cpp
	src/handlers/HCodeReviewOverlay.cpp
	src/nodes/NodeReviews.cpp
	src/nodes/NodeReviewsList.cpp
	src/nodes/ReviewComment.cpp
	src/items/VNodeReviews.cpp
	src/items/VNodeReviewsStyle.cpp
	src/items/VReviewComment.cpp
	src/items/VReviewCommentStyle.cpp
	src/orderings/Orderings.cpp
	src/groupings/UseAnalysisGroupings.cpp
	test/SimpleTest.cpp
)
envision_plugin(CodeReview OOInteraction VersionControlUI)
