ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror -std=c++0x

INCLUDEPATH += ./headers \
	./src \
	./test \
    $${ENVISION_ROOT_DIR}
TARGET = apideptest
DEFINES += APIDEPTEST_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} -lhelloworld
QT = core

TEMPLATE = lib
CONFIG += plugin \
    warn_on thread
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target \
    pluginmeta
HEADERS += src/apideptest.h
SOURCES += src/apideptest.cpp
