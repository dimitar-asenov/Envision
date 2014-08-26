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
    src/AlloyIntegrationPlugin.h
SOURCES += src/AlloyIntegrationException.cpp \
	src/AlloyIntegrationPlugin.cpp \
	test/SimpleTest.cpp
