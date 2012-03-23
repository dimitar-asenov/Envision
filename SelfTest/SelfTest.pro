ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror \
    -std=c++0x
INCLUDEPATH += ./src \
    ./test \
    $${ENVISION_ROOT_DIR}
TARGET = selftest
DEFINES += SELFTEST_LIBRARY
QT = core
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread \
    precompile_header
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target \
    pluginmeta
PRECOMPILED_HEADER = src/precompiled.h
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
