ENVISION_SRC_DIR = $$PWD/..
ENVISION_DIR = $${ENVISION_SRC_DIR}/envision_build
ENVISION_PLUGINS_DIR = $${ENVISION_DIR}/plugins
INCLUDEPATH += ./headers \
	./src \
	./test \
    $${ENVISION_SRC_DIR}/core/headers \
    $${ENVISION_SRC_DIR}
TARGET = apideptest
DEFINES += APIDEPTEST_LIBRARY
win32:LIBS += -L$${ENVISION_PLUGINS_DIR} -lhelloworld
QT = core
QMAKE_CXXFLAGS += -Werror
TEMPLATE = lib
CONFIG += plugin \
    warn_on thread
target.path = $$ENVISION_PLUGINS_DIR
pluginmeta.path = $$ENVISION_PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target \
    pluginmeta
debug:DEFINES += DEBUG
HEADERS += src/apideptest.h
SOURCES += src/apideptest.cpp
