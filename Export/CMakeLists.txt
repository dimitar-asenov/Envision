cmake_minimum_required(VERSION 3.2.2)

project(Export)
include(../CMake/common.cmake)

qt5_add_resources(Resource_Cpp "Export.qrc" OPTIONS "--no-compress")

add_library(Export SHARED
	${Resource_Cpp}
	src/writer/FileWriter.h
	src/writer/Exporter.h
	src/writer/TextToNodeMap.h
	src/writer/FragmentLayouter.h
	src/tree/CompositeFragment.h
	src/tree/TextFragment.h
	src/tree/SourceFragment.h
	src/tree/SourceFile.h
	src/tree/SourceDir.h
	src/precompiled.h
	src/ExportException.h
	src/export_api.h
	src/ExportPlugin.h
	src/ExportMapContainer.h
	src/ExportError.h
	src/visitor/Visitor.h
	src/visitor/VisitorData.h
	src/visitor/PrintContext.h
	test/LayoutTests.cpp
	src/writer/FileWriter.cpp
	test/GenerationTests.cpp
	src/writer/Exporter.cpp
	src/writer/TextToNodeMap.cpp
	src/writer/FragmentLayouter.cpp
	src/tree/CompositeFragment.cpp
	src/tree/TextFragment.cpp
	src/tree/SourceFragment.cpp
	src/tree/SourceFile.cpp
	src/tree/SourceDir.cpp
	src/ExportException.cpp
	src/ExportPlugin.cpp
	src/ExportMapContainer.cpp
	src/ExportError.cpp
	src/visitor/PrintContext.cpp
)
envision_plugin(Export ModelBase)