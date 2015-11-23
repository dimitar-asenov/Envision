TARGET = cppexport
include(../Core/common_plugin.pri)

DEFINES += CppExport_EXPORTS
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel

HEADERS += src/precompiled.h \
    src/CppExportException.h \
	src/cppexport_api.h \
	src/CppExportPlugin.h \
	src/dependency_analysis/DependencyAnalyzer.h \
	src/dependency_analysis/DependencyUnit.h \
	src/dependency_analysis/DependencyTarget.h \
	src/Config.h \
	src/dependency_analysis/DependencyComposite.h \
    src/exporter/CppExporter.h \
    src/exporter/ExportError.h \
    src/visitors/header_visitors/DeclarationVisitorHeader.h \
    src/visitors/header_visitors/ElementVisitorHeader.h \
    src/visitors/header_visitors/ExpressionVisitorHeader.h \
    src/visitors/header_visitors/StatementVisitorHeader.h \
    src/visitors/Visitor.h \
    src/visitors/header_visitors/VisitorDefs.h \
    src/visitors/source_visitors/DeclarationVisitorSource.h \
    src/visitors/source_visitors/ElementVisitorSource.h \
    src/visitors/source_visitors/ExpressionVisitorSource.h \
    src/visitors/source_visitors/StatementVisitorSource.h \
    src/visitors/source_visitors/VisitorDefs.h
SOURCES += src/CppExportException.cpp \
	src/CppExportPlugin.cpp \
	test/SimpleTest.cpp \
	src/dependency_analysis/DependencyAnalyzer.cpp \
	src/dependency_analysis/DependencyUnit.cpp \
	src/Config.cpp \
	src/dependency_analysis/DependencyComposite.cpp \
    src/exporter/CppExporter.cpp \
    src/exporter/ExportError.cpp \
    src/visitors/header_visitors/DeclarationVisitorHeader.cpp \
    src/visitors/header_visitors/ElementVisitorHeader.cpp \
    src/visitors/header_visitors/ExpressionVisitorHeader.cpp \
    src/visitors/header_visitors/StatementVisitorHeader.cpp \
    src/visitors/Visitor.cpp \
    src/visitors/source_visitors/DeclarationVisitorSource.cpp \
    src/visitors/source_visitors/ElementVisitorSource.cpp \
    src/visitors/source_visitors/ExpressionVisitorSource.cpp \
    src/visitors/source_visitors/StatementVisitorSource.cpp

## INSTALLING SPECIFICS:
config.files=config.json
config.path=$${BUILD_DIR}/cpp-export-settings

INSTALLS += config
