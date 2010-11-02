ENVISION_SRC_DIR = $$PWD/..
ENVISION_DIR = $${ENVISION_SRC_DIR}/envision_build
ENVISION_PLUGINS_DIR = $${ENVISION_DIR}/plugins
TEMPLATE = app
TARGET = Envision
target.path = $$ENVISION_DIR
INSTALLS += target
CONFIG += qt \
    thread \
    warn_on
QT += core \
    gui \
    xml
INCLUDEPATH += ./headers
QMAKE_CXXFLAGS += -Werror
HEADERS += src/TestEvent.h \
    src/TestRunner.h \
    src/DefaultEnvisionManager.h \
    src/EnvisionException.h \
    headers/EnvisionManager.h \
    headers/PluginInfo.h \
    src/PluginManager.h \
    headers/EnvisionPlugin.h \
    src/EnvisionWindow.h
SOURCES += src/TestEvent.cpp \
    src/TestRunner.cpp \
    src/DefaultEnvisionManager.cpp \
    src/EnvisionException.cpp \
    src/PluginManager.cpp \
    src/main.cpp \
    src/EnvisionWindow.cpp
