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

HEADERS += src/precompiled.h \
    src/JavaExportException.h \
	src/javaexport_api.h \
    src/JavaExportPlugin.h
SOURCES += src/JavaExportException.cpp \
	src/JavaExportPlugin.cpp \
	test/SimpleTest.cpp
