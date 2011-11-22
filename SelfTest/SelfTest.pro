ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror
INCLUDEPATH += ./headers \
    ./src \
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
PRECOMPILED_HEADER = headers/precompiled.h
HEADERS += headers/precompiled.h \
    headers/SelfTestSuite.h \
    headers/TestAssertions.h \
    headers/TestResult.h \
    headers/TestResults.h \
    headers/Test.h \
    headers/TestManager.h \
    headers/selftest_api.h \
    src/selftest.h
SOURCES += src/TestResult.cpp \
    src/TestResults.cpp \
    src/Test.cpp \
    src/selftest.cpp
