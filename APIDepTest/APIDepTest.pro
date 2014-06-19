TARGET = apideptest
include(../Core/common_plugin.pri)
DEFINES += APIDEPTEST_LIBRARY
win32:LIBS += -lhelloworld
QT = core

HEADERS += src/precompiled.h \
    src/APIDepTestPlugin.h
SOURCES += \
    src/APIDepTestPlugin.cpp
