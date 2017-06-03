cmake_minimum_required(VERSION 3.7.2)

project(JavaExport)
include(../CMake/common.cmake)

add_library(JavaExport SHARED
	src/commands/CJavaExport.h
	src/exporter/JavaExporter.h
	src/precompiled.h
	src/JavaExportException.h
	src/javaexport_api.h
	src/JavaExportPlugin.h
	src/visitors/DeclarationVisitor.h
	src/visitors/StatementVisitor.h
	src/visitors/ExpressionVisitor.h
	src/visitors/ElementVisitor.h
	src/commands/CJavaExport.cpp
	src/exporter/JavaExporter.cpp
	src/JavaExportException.cpp
	src/JavaExportPlugin.cpp
	test/SimpleTest.cpp
	src/visitors/DeclarationVisitor.cpp
	src/visitors/StatementVisitor.cpp
	src/visitors/ExpressionVisitor.cpp
	src/visitors/ElementVisitor.cpp
)

envision_plugin(JavaExport OOInteraction Export)