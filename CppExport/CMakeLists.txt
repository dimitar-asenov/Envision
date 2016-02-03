cmake_minimum_required(VERSION 3.2.2)

project(CppExport)
include(../CMake/common.cmake)

add_library(CppExport SHARED
	src/precompiled.h
	src/CppExportException.h
	src/cppexport_api.h
	src/CppExportPlugin.h
	src/Config.h
	src/exporter/CppExporter.h
	src/visitors/DeclarationVisitor.h
	src/visitors/ElementVisitor.h
	src/visitors/ExpressionVisitor.h
	src/visitors/StatementVisitor.h
	src/visitors/CppPrintContext.h
	src/CodeUnit.h
	src/CodeComposite.h
	src/CodeUnitPart.h
	src/ExportHelpers.h
	src/SpecialCases.h
	src/CppExportException.cpp
	src/CppExportPlugin.cpp
	test/SimpleTest.cpp
	src/Config.cpp
	src/exporter/CppExporter.cpp
	src/visitors/DeclarationVisitor.cpp
	src/visitors/ElementVisitor.cpp
	src/visitors/ExpressionVisitor.cpp
	src/visitors/StatementVisitor.cpp
	src/CodeUnit.cpp
	src/CodeComposite.cpp
	src/CodeUnitPart.cpp
	src/ExportHelpers.cpp
	src/SpecialCases.cpp
)

envision_plugin(CppExport OOModel Export OOInteraction)
install(FILES "config.json" DESTINATION ${BUILD_DIR}/cpp-export-settings)