cmake_minimum_required(VERSION 3.2.2)

project(InformationScripting)
include(../CMake/common.cmake)

find_package(PythonLibs 3 REQUIRED)

# Find libboost 3 for Ubuntu
if(UNIX)
	execute_process(COMMAND lsb_release -i OUTPUT_VARIABLE distribution_)
	if( "${distribution_}" MATCHES ".*Ubuntu.*")
		find_package(Boost REQUIRED python-py34)
		set(UBUNTU TRUE)
	endif()
endif()

# Find libboost for non-ubuntu
if(NOT UBUNTU)
	find_package(Boost REQUIRED python3)
endif()

include_directories(SYSTEM ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_library(InformationScripting SHARED
	src/precompiled.h
	src/InformationScriptingException.h
	src/informationscripting_api.h
	src/InformationScriptingPlugin.h
	src/queries/Query.h
	src/queries/AstQuery.h
	src/queries/CompositeQuery.h
	src/queries/NodePropertyAdder.h
	src/queries/UnionOperator.h
	src/queries/ScriptQuery.h
	src/dataformat/Tuple.h
	src/dataformat/TupleSet.h
	src/dataformat/Property.h
	src/queries/SubstractOperator.h
	src/queries/AddASTPropertiesAsTuples.h
	src/nodes/TagExtension.h
	src/queries/TagQuery.h
	src/queries/LinearQuery.h
	src/queries/VersionControlQuery.h
	src/queries/BreakpointManager.h
	src/queries/RuntimeQuery.h
	src/query_prompt/QueryPromptMode.h
	src/queries/PassthroughQuery.h
	src/query_prompt/nodes/CommandArgument.h
	src/query_prompt/nodes/CommandNode.h
	src/query_prompt/nodes/CompositeQueryNode.h
	src/query_prompt/nodes/OperatorQueryNode.h
	src/query_prompt/nodes/QueryNode.h
	src/query_prompt/nodes/QueryNodeContainer.h
	src/query_prompt/visualization/VOperatorQueryNodeStyle.h
	src/query_prompt/visualization/VOperatorQueryNode.h
	src/query_prompt/visualization/VCompositeQueryNodeStyle.h
	src/query_prompt/visualization/VCompositeQueryNode.h
	src/query_prompt/visualization/VCommandNode.h
	src/query_prompt/visualization/VCommandNodeStyle.h
	src/query_prompt/visualization/VCommandArgumentStyle.h
	src/query_prompt/visualization/VCommandArgument.h
	src/query_prompt/parsing/QueryBuilder.h
	src/query_prompt/visualization/VQueryNodeContainer.h
	src/query_prompt/visualization/QueryPromptInputStyle.h
	src/query_prompt/visualization/QueryPromptInput.h
	src/query_prompt/HQuery.h
	src/python_bindings/AstApi.h
	src/python_bindings/DataApi.h
	src/python_bindings/BoostPythonHelpers.h
	src/python_bindings/PythonSet.h
	src/python_bindings/AstModification.h
	src/query_framework/Optional.h
	src/query_framework/QueryRegistry.h
	src/query_framework/QueryExecutor.h
	src/query_framework/ArgumentParser.h
	src/query_framework/ArgumentRule.h
	src/query_framework/QueryResultVisualizer.h
	src/queries/Heatmap.h
	src/query_prompt/parsing/QueryParser.h
	src/query_framework/QueryParsingException.h
	src/queries/Count.h
	src/queries/Join.h
	src/queries/Filter.h
	src/queries/CanReach.h
	src/query_framework/QueryRuntimeException.h
	src/queries/Yield.h
	src/InformationScriptingException.cpp
	src/InformationScriptingPlugin.cpp
	test/SimpleTest.cpp
	src/queries/AstQuery.cpp
	src/queries/CompositeQuery.cpp
	src/queries/NodePropertyAdder.cpp
	src/queries/UnionOperator.cpp
	src/queries/ScriptQuery.cpp
	src/dataformat/Tuple.cpp
	src/dataformat/TupleSet.cpp
	src/dataformat/Property.cpp
	src/queries/SubstractOperator.cpp
	src/queries/AddASTPropertiesAsTuples.cpp
	src/nodes/TagExtension.cpp
	src/queries/TagQuery.cpp
	src/queries/LinearQuery.cpp
	src/queries/VersionControlQuery.cpp
	src/queries/BreakpointManager.cpp
	src/queries/RuntimeQuery.cpp
	src/query_prompt/QueryPromptMode.cpp
	src/queries/PassthroughQuery.cpp
	src/query_prompt/nodes/CommandArgument.cpp
	src/query_prompt/nodes/CommandNode.cpp
	src/query_prompt/nodes/CompositeQueryNode.cpp
	src/query_prompt/nodes/OperatorQueryNode.cpp
	src/query_prompt/nodes/QueryNode.cpp
	src/query_prompt/nodes/QueryNodeContainer.cpp
	src/query_prompt/visualization/QueryPromptInput.cpp
	src/query_prompt/visualization/QueryPromptInputStyle.cpp
	src/query_prompt/parsing/QueryBuilder.cpp
	src/query_prompt/visualization/VQueryNodeContainer.cpp
	src/query_prompt/visualization/VOperatorQueryNodeStyle.cpp
	src/query_prompt/visualization/VOperatorQueryNode.cpp
	src/query_prompt/visualization/VCompositeQueryNodeStyle.cpp
	src/query_prompt/visualization/VCompositeQueryNode.cpp
	src/query_prompt/visualization/VCommandNodeStyle.cpp
	src/query_prompt/visualization/VCommandArgumentStyle.cpp
	src/query_prompt/visualization/VCommandArgument.cpp
	src/query_prompt/visualization/VCommandNode.cpp
	src/query_prompt/HQuery.cpp
	src/python_bindings/AstApi_Generated.cpp
	src/python_bindings/DataApi.cpp
	src/python_bindings/BoostPythonHelpers.cpp
	src/python_bindings/PythonSet.cpp
	src/python_bindings/AstModification.cpp
	src/query_framework/QueryExecutor.cpp
	src/query_framework/QueryRegistry.cpp
	src/query_framework/ArgumentParser.cpp
	src/query_framework/ArgumentRule.cpp
	src/query_framework/QueryResultVisualizer.cpp
	src/queries/Heatmap.cpp
	src/query_prompt/parsing/QueryParser.cpp
	src/query_framework/QueryParsingException.cpp
	src/queries/Count.cpp
	src/queries/Join.cpp
	src/queries/Filter.cpp
	src/queries/CanReach.cpp
	src/query_framework/QueryRuntimeException.cpp
	src/queries/Yield.cpp
	scripts/color.py
	scripts/filterCasts.py
	scripts/heatmapDataGen.py
	scripts/not.py
	scripts/sink.py
	scripts/debug.py
	scripts/filterMultiplications.py
	scripts/ifToWhile.py
	scripts/profileSource.py
	scripts/table.py
)

target_link_libraries(InformationScripting ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})
envision_plugin(InformationScripting OOInteraction OODebug)

