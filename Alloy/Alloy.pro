TARGET = alloy
include(../Core/common_plugin.pri)

DEFINES += Alloy_EXPORTS
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -loointeraction \
    -lcontractslibrary

HEADERS += src/precompiled.h \
    src/AlloyException.h \
	src/alloy_api.h \
    src/AlloyPlugin.h \
    src/commands/CAlloy.h \
    src/visitors/AlloyVisitor.h \
    src/exporter/AlloyExporter.h
SOURCES += src/AlloyException.cpp \
	src/AlloyPlugin.cpp \
	test/SimpleTest.cpp \
    src/commands/CAlloy.cpp \
    src/visitors/AlloyVisitor.cpp \
    src/exporter/AlloyExporter.cpp

alloy.path = $${BUILD_DIR}/alloy
alloy.files = alloy_data/*

INSTALLS += alloy
