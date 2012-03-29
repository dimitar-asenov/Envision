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
TARGET = logger
DEFINES += LOGGER_LIBRARY
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
HEADERS += src/Log.h \
    src/logger_api.h \
    src/precompiled.h \
    test/LogTester.h \
    src/DefaultLog.h \
    src/logger.h
SOURCES += test/LogTester.cpp \
    src/DefaultLog.cpp \
    src/Log.cpp \
    src/logger.cpp
