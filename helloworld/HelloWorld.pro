ENVISION_SRC_DIR = $$PWD/..
ENVISION_DIR = $${ENVISION_SRC_DIR}/envision_build
ENVISION_PLUGINS_DIR = $${ENVISION_DIR}/plugins
INCLUDEPATH += ./headers \
    $${ENVISION_SRC_DIR}/core/headers \
    $${ENVISION_SRC_DIR}
TARGET = helloworld
DEFINES += HELLOWORLD_LIBRARY
QT = core
QMAKE_CXXFLAGS += -Werror
TEMPLATE = lib
CONFIG += plugin warn_on thread
target.path = $$ENVISION_PLUGINS_DIR
pluginmeta.path = $$ENVISION_PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target pluginmeta
HEADERS += headers/HelloWorldPrinter.h \
    headers/helloworld_api.h \
    src/helloworld.h
SOURCES += src/HelloWorldPrinter.cpp \
    src/helloworld.cpp
