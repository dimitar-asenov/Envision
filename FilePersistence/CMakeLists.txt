cmake_minimum_required(VERSION 3.2.2)

project(FilePersistence)
include(../CMake/common.cmake)

#We need to have a file there when running CMake
set(QRC_FILE "${CMAKE_CURRENT_BINARY_DIR}/FilePersistence.qrc")
if(NOT EXISTS ${QRC_FILE})
	file(WRITE ${QRC_FILE} "<!DOCTYPE RCC><RCC version=\"1.0\"><qresource/></RCC>\n")
endif()
qt5_add_resources(Resource_Cpp ${QRC_FILE} OPTIONS "--no-compress")
#keep resources fresh
add_custom_target(FilePersistence_UpdateResources
	COMMAND ${ENVISION_ROOT_DIR}/misc/qt_resource_file_from_dir.py  test/persisted  /FilePersistence/test/persisted  ${QRC_FILE}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


add_library(FilePersistence SHARED
	${Resource_Cpp}
	src/simple/GenericPersistentUnit.h
	src/simple/GenericTree.h
	src/simple/Parser.h
	src/simple/GenericNode.h
	src/simple/SimpleTextFileStore.h
	src/simple/PiecewiseLoader.h
	src/FilePersistenceException.h
	src/SystemClipboard.h
	src/XMLModel.h
	src/filepersistence_api.h
	src/precompiled.h
	src/FilePersistencePlugin.h
	src/version_control/ChangeDescription.h
	src/version_control/Commit.h
	src/version_control/CommitGraph.h
	src/version_control/Diff.h
	src/version_control/GitRepository.h
	src/version_control/History.h
	src/version_control/MergeV1.h
	src/version_control/ChangeDependencyGraph.h
	src/version_control/ConflictPairs.h
	src/version_control/ConflictUnitDetector.h
	src/version_control/ListMergeComponent.h
	src/version_control/ConflictPipelineComponent.h
	src/version_control/ConflictPipelineComponent.cpp
	src/version_control/LinkedChangesTransition.h
	src/version_control/GitPiecewiseLoader.h
	src/version_control/LinkedChangesSet.h
	src/version_control/Diff3Parse.h
	src/utils/FileUtil.h
	test/VCTestProject.h
	src/simple/GenericPersistentUnit.cpp
	src/simple/GenericTree.cpp
	src/simple/Parser.cpp
	src/simple/GenericNode.cpp
	src/simple/SimpleTextFileStore.cpp
	src/simple/PiecewiseLoader.cpp
	test/TypedListTests.cpp
	test/ClipboardTests.cpp
	src/XMLModel.cpp
	src/SystemClipboard.cpp
	test/PartialNodeTests.cpp
	src/FilePersistenceException.cpp
	test/LoadTests.cpp
	test/SaveTests.cpp
	src/FilePersistencePlugin.cpp
	src/version_control/ChangeDescription.cpp
	src/version_control/Diff.cpp
	src/version_control/GitRepository.cpp
	test/VersionControlDiffTests.cpp
	src/version_control/CommitGraph.cpp
	src/version_control/History.cpp
	src/version_control/Commit.cpp
	src/version_control/MergeV1.cpp
	src/version_control/ChangeDependencyGraph.cpp
	src/version_control/ConflictPairs.cpp
	src/version_control/ConflictUnitDetector.cpp
	src/version_control/ListMergeComponent.cpp
	test/VersionControlMergetests.cpp
	src/version_control/LinkedChangesTransition.cpp
	src/version_control/GitPiecewiseLoader.cpp
	src/version_control/LinkedChangesSet.cpp
	src/version_control/Diff3Parse.cpp
	test/VCTestProject.cpp
	src/version_control/merge/change_graph/ChangeGraph.h
	src/version_control/merge/change_graph/ChangeGraph.cpp
	src/version_control/merge/change_graph/MergeChange.cpp
	src/version_control/merge/change_graph/MergeChange.h
	src/version_control/merge/change_graph/Dependencies.h
	src/version_control/merge/change_graph/Dependencies.cpp
	src/version_control/merge/change_graph/Conflicts.h
	src/version_control/merge/change_graph/Conflicts.cpp
	src/version_control/merge/pipeline_components/ConflictUnitComponent.cpp
	src/version_control/merge/pipeline_components/ConflictUnitComponent.h
	src/version_control/merge/pipeline_components/ListMergeComponentV2.cpp
	src/version_control/merge/pipeline_components/ListMergeComponentV2.h
	src/version_control/merge/pipeline_components/MergePipelineComponent.cpp
	src/version_control/merge/pipeline_components/MergePipelineComponent.h
	src/version_control/merge/MergeV2.cpp
	src/version_control/merge/MergeV2.h
	src/version_control/merge/MergeData.cpp
	src/version_control/merge/MergeData.h
	src/version_control/Merge.h
	src/version_control/merge/SoftConflict.h
	src/version_control/merge/SoftConflict.cpp
	src/version_control/merge/pipeline_components/DiscardConflictingDeletesComponent.h
	src/version_control/merge/pipeline_components/DiscardConflictingDeletesComponent.cpp
)
add_dependencies(FilePersistence FilePersistence_UpdateResources)

find_package(Qt5 REQUIRED Xml)
target_link_libraries(FilePersistence Qt5::Xml -lgit2)

envision_plugin(FilePersistence ModelBase)
