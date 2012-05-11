include(common.pri)

TEMPLATE = app
TARGET = Envision
target.path = $$BUILD_DIR
INSTALLS += target
DEFINES += CORE_LIBRARY
QT += xml

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
