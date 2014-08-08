TARGET = javaexport
include(../Core/common_plugin.pri)
DEFINES += JAVAEXPORT_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -lexport \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -loointeraction
HEADERS += src/commands/CJavaExport.h \
    src/exporter/ExportError.h \
    src/exporter/JavaExporter.h \
    src/visitors/TopLevelVisitor.h \
    src/precompiled.h \
    src/JavaExportException.h \
    src/javaexport_api.h \
    src/JavaExportPlugin.h
SOURCES += src/commands/CJavaExport.cpp \
    src/exporter/ExportError.cpp \
    src/exporter/JavaExporter.cpp \
    src/visitors/TopLevelVisitor.cpp \
    src/JavaExportException.cpp \
    src/JavaExportPlugin.cpp \
    test/SimpleTest.cpp
