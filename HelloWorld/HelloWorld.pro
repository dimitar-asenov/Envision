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
TARGET = helloworld
DEFINES += HELLOWORLD_LIBRARY
QT = core
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread \
    precompile_header
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target pluginmeta
PRECOMPILED_HEADER = headers/precompiled.h
HEADERS += headers/precompiled.h \
    headers/HelloWorldPrinter.h \
    headers/helloworld_api.h \
    src/helloworld.h
SOURCES += src/HelloWorldPrinter.cpp \
    src/helloworld.cpp
