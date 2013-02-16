TARGET = helloworld
include(../Core/common_plugin.pri)

TEMPLATE=lib

CONFIG+=debug_and_release

DEFINES += HELLOWORLD_LIBRARY
QT = core

HEADERS += src/HelloWorldPrinter.h \
    src/helloworld_api.h \
    src/precompiled.h \
    src/helloworld.h
SOURCES += src/HelloWorldPrinter.cpp \
    src/helloworld.cpp
