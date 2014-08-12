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
    src/precompiled.h \
    src/JavaExportException.h \
    src/javaexport_api.h \
    src/JavaExportPlugin.h \
    src/visitors/DeclarationVisitor.h \
    src/visitors/Visitor.h \
    src/visitors/StatementVisitor.h \
    src/visitors/ExpressionVisitor.h \
    src/visitors/ElementVisitor.h \
    src/visitors/VisitorDefs.h
SOURCES += src/commands/CJavaExport.cpp \
    src/exporter/ExportError.cpp \
    src/exporter/JavaExporter.cpp \
    src/JavaExportException.cpp \
    src/JavaExportPlugin.cpp \
    test/SimpleTest.cpp \
    src/visitors/DeclarationVisitor.cpp \
    src/visitors/Visitor.cpp \
    src/visitors/StatementVisitor.cpp \
    src/visitors/ExpressionVisitor.cpp \
    src/visitors/ElementVisitor.cpp
