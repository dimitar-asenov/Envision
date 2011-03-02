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
TARGET = controlflowvisualization
DEFINES += CONTROLFLOWVISUALIZATION_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
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
HEADERS += headers/items/VBlockCFStyle.h \
    headers/items/VBlockCF.h \
    headers/items/VReturnStatementCF.h \
    headers/items/VIfStatementCFStyle.h \
    headers/items/VIfStatementCF.h \
    headers/items/VLoopStatementCFStyle.h \
    headers/items/VLoopStatementCF.h \
    headers/items/VBreakStatementCF.h \
    headers/items/VContinueStatementCF.h \
    headers/items/VListCFStyle.h \
    headers/items/VListCF.h \
    headers/allCFVisualizations.h \
    headers/items/VMethodCFStyle.h \
    headers/items/VMethodCF.h \
    headers/items/ControlFlowItemStyle.h \
    headers/items/ControlFlowItem.h \
    headers/ControlFlowVisualizationException.h \
    headers/controlflowvisualization_api.h \
    src/controlflowvisualization.h
SOURCES += src/items/VBlockCF.cpp \
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
