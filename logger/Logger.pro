ENVISION_SRC_DIR = $$PWD/..
ENVISION_DIR = $${ENVISION_SRC_DIR}/envision_build
ENVISION_PLUGINS_DIR = $${ENVISION_DIR}/plugins
INCLUDEPATH += ./headers \
    $${ENVISION_SRC_DIR}/core/headers \
    $${ENVISION_SRC_DIR}
TARGET = logger
DEFINES += LOGGER_LIBRARY
QT = core
QMAKE_CXXFLAGS += -Werror
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread
target.path = $$ENVISION_PLUGINS_DIR
pluginmeta.path = $$ENVISION_PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target \
    pluginmeta
HEADERS += src/logger.h \
    headers/logger_api.h
SOURCES += src/logger.cpp
