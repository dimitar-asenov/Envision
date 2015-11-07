include(common.pri)
TEMPLATE = lib
TARGET = core
target.path = $$BUILD_DIR
INSTALLS += target
DEFINES += Core_EXPORTS
CONFIG += plugin
QT += xml
HEADERS += src/reflect/TypeRegistry.h \
    src/Profiler.h \
    src/reflect/typeIdMacros.h \
    src/reflect/Reflect.h \
    src/AdapterManager.h \
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
SOURCES += src/reflect/TypeRegistry.cpp \
    src/Profiler.cpp \
    src/AdapterManager.cpp \
    src/EnvisionException.cpp \
    src/EnvisionManager.cpp \
    src/core.cpp \
    src/EnvisionApplication.cpp \
    src/TestEvent.cpp \
    src/TestRunner.cpp \
    src/PluginManager.cpp \
    src/EnvisionWindow.cpp \
    src/global.cpp
