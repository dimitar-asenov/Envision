ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror \
    -std=c++0x
TEMPLATE = app
TARGET = Envision
target.path = $$BUILD_DIR
INSTALLS += target
DEFINES += CORE_LIBRARY
CONFIG += qt \
    thread \
    warn_on \
    precompile_header
QT += core \
    gui \
    xml
INCLUDEPATH += ./src \
    ./test
PRECOMPILED_HEADER = src/precompiled.h
HEADERS += src/EnvisionException.h \
    src/EnvisionManager.h \
    src/EnvisionPlugin.h \
    src/PluginInfo.h \
    src/global.h \
    src/precompiled.h \
    src/TestEvent.h \
    src/TestRunner.h \
    src/DefaultEnvisionManager.h \
    src/PluginManager.h \
    src/EnvisionWindow.h
SOURCES += src/TestEvent.cpp \
    src/TestRunner.cpp \
    src/DefaultEnvisionManager.cpp \
    src/PluginManager.cpp \
    src/main.cpp \
    src/EnvisionWindow.cpp
