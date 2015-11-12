TARGET = helloworld
include(../Core/common_plugin.pri)
DEFINES += HelloWorld_EXPORTS
QT = core

HEADERS += src/HelloWorldPrinter.h \
    src/helloworld_api.h \
    src/precompiled.h \
    src/HelloWorldPlugin.h
SOURCES += src/HelloWorldPrinter.cpp \
    src/HelloWorldPlugin.cpp
