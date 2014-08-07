TARGET = export
include(../Core/common_plugin.pri)

DEFINES += EXPORT_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase

HEADERS += src/precompiled.h \
    src/ExportException.h \
	src/export_api.h \
    src/ExportPlugin.h
SOURCES += src/ExportException.cpp \
	src/ExportPlugin.cpp \
	test/SimpleTest.cpp
