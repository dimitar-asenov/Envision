TARGET = apideptest
include(../Core/common_plugin.pri)

TEMPLATE=lib

CONFIG+=debug_and_release

DEFINES += APIDEPTEST_LIBRARY
win32:LIBS += -lhelloworld
QT = core

HEADERS += src/precompiled.h \
    src/apideptest.h
SOURCES += src/apideptest.cpp
