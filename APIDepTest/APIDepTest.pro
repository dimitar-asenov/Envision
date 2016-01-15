TARGET = apideptest
include(../Core/common_plugin.pri)
DEFINES += APIDepTest_EXPORTS
win32:LIBS += -lhelloworld
QT = core

HEADERS += src/precompiled.h \
    src/APIDepTestPlugin.h \
    src/apideptest_api.h
SOURCES += \
    src/APIDepTestPlugin.cpp
