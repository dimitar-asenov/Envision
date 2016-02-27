cmake_minimum_required(VERSION 3.2.2)

project(CppImport)
include(../CMake/common.cmake)

execute_process(COMMAND ${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --includedir
	OUTPUT_VARIABLE clangIncludes_)
string(STRIP "${clangIncludes_}" clangIncludes_)
include_directories(SYSTEM ${clangIncludes_})

execute_process(COMMAND ${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --ldflags
	OUTPUT_VARIABLE clangLibPath_)
string(STRIP "${clangLibPath_}" clangLibPath_)
string(SUBSTRING "${clangLibPath_}" 2 -1 clangLibPath_) #skip the -L
link_directories(${clangLibPath_})

execute_process(COMMAND ${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --libs cppbackend analysis codegen bitreader option core
	OUTPUT_VARIABLE clangLibs_)
string(STRIP "${clangLibs_}" clangLibs_)
set(clangLibs_ -lclangTooling -lclangFrontendTool -lclangFrontend -lclangDriver -lclangSerialization -lclangParse -lclangSema -lclangAnalysis -lclangEdit -lclangAST -lclangLex -lclangBasic ${clangLibs_})

execute_process(COMMAND ${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --system-libs
	OUTPUT_VARIABLE clangDependencies_)
string(STRIP "${clangDependencies_}" clangDependencies_)

add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

add_library(CppImport SHARED
	src/precompiled.h
	src/CppImportException.h
	src/cppimport_api.h
	src/ClangAstConsumer.h
	src/visitors/ClangAstVisitor.h
	src/CppImportManager.h
	src/CppImportUtilities.h
	src/CppImportLogger.h
	src/visitors/ExpressionVisitor.h
	src/ClangFrontendActionFactory.h
	src/visitors/TemplateArgumentVisitor.h
	src/manager/NodeHasher.h
	src/manager/TranslateManager.h
	src/TranslateFrontendAction.h
	src/CppImportPlugin.h
	src/comments/CommentParser.h
	src/macro/MacroDefinitions.h
	src/macro/MacroExpansions.h
	src/macro/MacroArgumentInfo.h
	src/macro/MacroArgumentLocation.h
	src/macro/MacroExpansion.h
	src/macro/PPCallback.h
	src/macro/MacroImporter.h
	src/macro/StandardMetaDefinitions.h
	src/macro/AllMetaDefinitions.h
	src/macro/NodeHelpers.h
	src/ClangHelpers.h
	src/EnvisionToClangMap.h
	src/macro/NodeToCloneMap.h
	src/Comment.h
	src/SpecialCases.h
	src/CppImportException.cpp
	test/SimpleTest.cpp
	src/ClangAstConsumer.cpp
	src/visitors/ClangAstVisitor.cpp
	src/CppImportManager.cpp
	src/CppImportUtilities.cpp
	src/CppImportLogger.cpp
	src/visitors/ExpressionVisitor.cpp
	src/ClangFrontendActionFactory.cpp
	src/visitors/TemplateArgumentVisitor.cpp
	src/manager/NodeHasher.cpp
	src/manager/TranslateManager.cpp
	src/TranslateFrontendAction.cpp
	src/CppImportPlugin.cpp
	src/comments/CommentParser.cpp
	src/macro/MacroDefinitions.cpp
	src/macro/MacroExpansions.cpp
	src/macro/MacroExpansion.cpp
	src/macro/PPCallback.cpp
	src/macro/MacroImporter.cpp
	src/macro/StandardMetaDefinitions.cpp
	src/macro/AllMetaDefinitions.cpp
	src/macro/NodeHelpers.cpp
	src/ClangHelpers.cpp
	src/EnvisionToClangMap.cpp
	src/macro/NodeToCloneMap.cpp
	src/Comment.cpp
	src/SpecialCases.cpp
)

target_link_libraries(CppImport ${clangLibs_} ${clangDependencies_})
envision_plugin(CppImport OOInteraction CppExport)
