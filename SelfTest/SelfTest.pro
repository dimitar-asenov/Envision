TARGET = selftest
include(../Core/common_plugin.pri)
DEFINES += SELFTEST_LIBRARY
QT = core

HEADERS += src/SelfTestSuite.h \
    src/Test.h \
    src/TestAssertions.h \
    src/TestManager.h \
    src/TestResult.h \
    src/TestResults.h \
    src/precompiled.h \
    src/selftest_api.h \
    src/selftest.h
SOURCES += src/TestResult.cpp \
    src/TestResults.cpp \
    src/Test.cpp \
    src/selftest.cpp
