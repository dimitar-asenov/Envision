TARGET = selftest
include(../Core/common_plugin.pri)
DEFINES += SelfTest_EXPORTS
QT = core

HEADERS += src/SelfTestSuite.h \
    src/Test.h \
    src/TestAssertions.h \
    src/TestManager.h \
    src/TestResult.h \
    src/TestResults.h \
    src/precompiled.h \
    src/selftest_api.h \
    src/SelfTestPlugin.h
SOURCES += src/TestResult.cpp \
    src/TestResults.cpp \
    src/Test.cpp \
    src/SelfTestPlugin.cpp
