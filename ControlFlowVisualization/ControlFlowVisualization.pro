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
TARGET = controlflowvisualization
DEFINES += CONTROLFLOWVISUALIZATION_LIBRARY
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
HEADERS += src/ControlFlowVisualizationException.h \
    src/allCFVisualizations.h \
    src/controlflowvisualization_api.h \
    src/handlers/HControlFlowMethodSwitch.h \
    src/items/ControlFlowItem.h \
    src/items/ControlFlowItemStyle.h \
    src/items/VBlockCF.h \
    src/items/VBlockCFStyle.h \
    src/items/VBreakStatementCF.h \
    src/items/VContinueStatementCF.h \
    src/items/VControlFlowMethodSwitch.h \
    src/items/VIfStatementCF.h \
    src/items/VIfStatementCFStyle.h \
    src/items/VListCF.h \
    src/items/VListCFStyle.h \
    src/items/VLoopStatementCF.h \
    src/items/VLoopStatementCFStyle.h \
    src/items/VMethodCF.h \
    src/items/VMethodCFStyle.h \
    src/items/VReturnStatementCF.h \
    src/precompiled.h \
    src/controlflowvisualization.h
SOURCES += src/handlers/HControlFlowMethodSwitch.cpp \
    src/items/VControlFlowMethodSwitch.cpp \
    src/items/VBlockCF.cpp \
    src/items/VBlockCFStyle.cpp \
    src/items/VReturnStatementCF.cpp \
    src/items/VIfStatementCF.cpp \
    src/items/VIfStatementCFStyle.cpp \
    src/items/VLoopStatementCF.cpp \
    src/items/VLoopStatementCFStyle.cpp \
    src/items/VBreakStatementCF.cpp \
    src/items/VContinueStatementCF.cpp \
    src/items/VListCF.cpp \
    src/items/VListCFStyle.cpp \
    src/items/VMethodCF.cpp \
    src/items/VMethodCFStyle.cpp \
    src/items/ControlFlowItem.cpp \
    src/items/ControlFlowItemStyle.cpp \
    src/ControlFlowVisualizationException.cpp \
    src/controlflowvisualization.cpp \
    test/SimpleTest.cpp
