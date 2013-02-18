TARGET = helloworld
include(../Core/common_plugin.pri)
DEFINES += HELLOWORLD_LIBRARY
QT = core

HEADERS += src/HelloWorldPrinter.h \
    src/helloworld_api.h \
    src/precompiled.h \
    src/helloworld.h
SOURCES += src/HelloWorldPrinter.cpp \
    src/helloworld.cpp
