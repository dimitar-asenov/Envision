TARGET = logger
include(../Core/common_plugin.pri)
DEFINES += LOGGER_LIBRARY
QT = core
HEADERS += src/Timer.h \
    src/Log.h \
    src/logger_api.h \
    src/precompiled.h \
    test/LogTester.h \
    src/DefaultLog.h \
    src/logger.h
SOURCES += src/Timer.cpp \
    test/LogTester.cpp \
    src/DefaultLog.cpp \
    src/Log.cpp \
    src/logger.cpp
