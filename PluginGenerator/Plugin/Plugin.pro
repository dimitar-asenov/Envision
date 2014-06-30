TARGET = PLUGINNAME_LOWERCASE
include(../Core/common_plugin.pri)

DEFINES += PLUGINNAME_UPPERCASE_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase

HEADERS += src/precompiled.h \
    src/PLUGINNAMEException.h \
	src/PLUGINNAME_LOWERCASE_api.h \
    src/PLUGINNAMEPlugin.h
SOURCES += src/PLUGINNAMEException.cpp \
	src/PLUGINNAMEPlugin.cpp \
	test/SimpleTest.cpp
