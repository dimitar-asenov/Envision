TARGET = alloyintegration
include(../Core/common_plugin.pri)

DEFINES += ALLOYINTEGRATION_LIBRARY
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
    src/AlloyIntegrationException.h \
	src/alloyintegration_api.h \
    src/AlloyIntegrationPlugin.h \
    src/commands/CAlloy.h \
    src/visitors/AlloyVisitor.h \
    src/exporter/AlloyExporter.h
SOURCES += src/AlloyIntegrationException.cpp \
	src/AlloyIntegrationPlugin.cpp \
	test/SimpleTest.cpp \
    src/commands/CAlloy.cpp \
    src/visitors/AlloyVisitor.cpp \
    src/exporter/AlloyExporter.cpp

alloy.path = $${BUILD_DIR}/alloy
alloy.files = alloy_data/*

INSTALLS += alloy
