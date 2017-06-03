cmake_minimum_required(VERSION 3.7.2)

project(ControlFlowVisualization)
include(../CMake/common.cmake)

add_library(ControlFlowVisualization SHARED
	src/ControlFlowVisualizationException.h
	src/allCFVisualizations.h
	src/controlflowvisualization_api.h
	src/items/ControlFlowItem.h
	src/items/ControlFlowItemStyle.h
	src/items/VBlockCF.h
	src/items/VBlockCFStyle.h
	src/items/VBreakStatementCF.h
	src/items/VContinueStatementCF.h
	src/items/VIfStatementCF.h
	src/items/VIfStatementCFStyle.h
	src/items/VListCF.h
	src/items/VListCFStyle.h
	src/items/VLoopStatementCF.h
	src/items/VLoopStatementCFStyle.h
	src/items/VReturnStatementCF.h
	src/precompiled.h
	src/ControlFlowVisualizationPlugin.h
	src/items/VBlockCF.cpp
	src/items/VBlockCFStyle.cpp
	src/items/VReturnStatementCF.cpp
	src/items/VIfStatementCF.cpp
	src/items/VIfStatementCFStyle.cpp
	src/items/VLoopStatementCF.cpp
	src/items/VLoopStatementCFStyle.cpp
	src/items/VBreakStatementCF.cpp
	src/items/VContinueStatementCF.cpp
	src/items/VListCF.cpp
	src/items/VListCFStyle.cpp
	src/items/ControlFlowItem.cpp
	src/items/ControlFlowItemStyle.cpp
	src/ControlFlowVisualizationException.cpp
	test/SimpleTest.cpp
	src/ControlFlowVisualizationPlugin.cpp

)

envision_plugin(ControlFlowVisualization OOInteraction)