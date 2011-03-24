ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror
INCLUDEPATH += ./headers \
    ./src \
    ./test \
    $${ENVISION_ROOT_DIR}/Core/headers \
    $${ENVISION_ROOT_DIR}
TARGET = custommethodcall
DEFINES += CUSTOMMETHODCALL_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization
QT = core
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
styles.path = $${BUILD_DIR}/styles
styles.files = styles/*
INSTALLS += target \
    pluginmeta \
    styles
HEADERS += headers/items/FindMethodVisStyle.h \
    headers/items/FindMethodVis.h \
    headers/CustomVisualization.h \
    headers/CustomMethodCallException.h \
    headers/custommethodcall_api.h \
    src/custommethodcall.h
SOURCES += src/items/FindMethodVis.cpp \
    src/items/FindMethodVisStyle.cpp \
    src/CustomVisualization.cpp \
    src/CustomMethodCallException.cpp \
    src/custommethodcall.cpp \
    test/SimpleTest.cpp
