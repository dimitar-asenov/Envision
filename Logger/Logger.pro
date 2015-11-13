TARGET = logger
include(../Core/common_plugin.pri)
DEFINES += Logger_EXPORTS
QT = core
HEADERS += src/Timer.h \
    src/Log.h \
    src/logger_api.h \
    src/precompiled.h \
    test/LogTester.h \
    src/DefaultLog.h \
    src/LoggerPlugin.h
SOURCES += src/Timer.cpp \
    test/LogTester.cpp \
    src/DefaultLog.cpp \
    src/Log.cpp \
    src/LoggerPlugin.cpp
