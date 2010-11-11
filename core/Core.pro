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
./src ./test QMAKE_CXXFLAGS += -Werror
debug:DEFINES += DEBUG
HEADERS += headers/EnvisionException.h \
    src/TestEvent.h \
    src/TestRunner.h \
    src/DefaultEnvisionManager.h \
    headers/EnvisionManager.h \
    headers/PluginInfo.h \
    src/PluginManager.h \
    headers/EnvisionPlugin.h \
    src/EnvisionWindow.h
SOURCES += src/TestEvent.cpp \
    src/TestRunner.cpp \
    src/DefaultEnvisionManager.cpp \
    src/PluginManager.cpp \
    src/main.cpp \
    src/EnvisionWindow.cpp
