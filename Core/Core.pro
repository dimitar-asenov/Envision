include(common.pri)
TEMPLATE = lib
TARGET = core
target.path = $$BUILD_DIR
INSTALLS += target
DEFINES += CORE_LIBRARY
CONFIG += plugin
QT += xml
HEADERS += src/AdapterManager.h \
    src/InitializationRegistry.h \
    src/core_api.h \
    src/core.h \
    src/EnvisionApplication.h \
    src/EnvisionException.h \
    src/EnvisionManager.h \
    src/EnvisionPlugin.h \
    src/PluginInfo.h \
    src/global.h \
    src/precompiled.h \
    src/TestEvent.h \
    src/TestRunner.h \
    src/PluginManager.h \
    src/EnvisionWindow.h
SOURCES += src/AdapterManager.cpp \
    src/InitializationRegistry.cpp \
    src/EnvisionException.cpp \
    src/EnvisionManager.cpp \
    src/core.cpp \
    src/EnvisionApplication.cpp \
    src/TestEvent.cpp \
    src/TestRunner.cpp \
    src/PluginManager.cpp \
    src/EnvisionWindow.cpp
