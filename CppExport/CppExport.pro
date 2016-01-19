TARGET = cppexport
include(../Core/common_plugin.pri)

DEFINES += CppExport_EXPORTS
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -loointeraction

HEADERS += src/precompiled.h \
    src/CppExportException.h \
	src/cppexport_api.h \
	src/CppExportPlugin.h \
	src/Config.h \
	src/exporter/CppExporter.h \
	src/CodeUnit.h \
	src/CodeComposite.h \
	src/CodeUnitPart.h \
	src/visitors/DeclarationVisitor.h \
	src/visitors/ElementVisitor.h \
	src/visitors/ExpressionVisitor.h \
	src/visitors/StatementVisitor.h
SOURCES += src/CppExportException.cpp \
	src/CppExportPlugin.cpp \
	test/SimpleTest.cpp \
	src/Config.cpp \
	src/exporter/CppExporter.cpp \
	src/CodeUnit.cpp \
	src/CodeComposite.cpp \
	src/CodeUnitPart.cpp \
	src/visitors/DeclarationVisitor.cpp \
	src/visitors/ElementVisitor.cpp \
	src/visitors/ExpressionVisitor.cpp \
	src/visitors/StatementVisitor.cpp

## INSTALLING SPECIFICS:
config.files=config.json
config.path=$${BUILD_DIR}/cpp-export-settings

INSTALLS += config
