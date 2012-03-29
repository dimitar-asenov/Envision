ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror \
    -std=c++0x
INCLUDEPATH += ./src \
    ./test \
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
QT = core \
    gui
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread \
    precompile_header
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
styles.path = $${BUILD_DIR}/styles
styles.files = styles/*
INSTALLS += target \
    pluginmeta \
    styles
PRECOMPILED_HEADER = src/precompiled.h
HEADERS += src/CustomMethodCallException.h \
    src/CustomVisualization.h \
    src/custommethodcall_api.h \
    src/items/EmptyMethodVis.h \
    src/items/EmptyMethodVisStyle.h \
    src/items/FindMethodVis.h \
    src/items/FindMethodVisStyle.h \
    src/items/InsertMethodVis.h \
    src/items/InsertMethodVisStyle.h \
    src/items/SumMethodVis.h \
    src/items/SumMethodVisStyle.h \
    src/precompiled.h \
    src/custommethodcall.h
SOURCES += src/items/SumMethodVis.cpp \
    src/items/SumMethodVisStyle.cpp \
    src/items/InsertMethodVis.cpp \
    src/items/InsertMethodVisStyle.cpp \
    src/items/EmptyMethodVisStyle.cpp \
    src/items/EmptyMethodVis.cpp \
    src/items/FindMethodVis.cpp \
    src/items/FindMethodVisStyle.cpp \
    src/CustomVisualization.cpp \
    src/CustomMethodCallException.cpp \
    src/custommethodcall.cpp \
    test/SimpleTest.cpp
