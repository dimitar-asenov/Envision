TARGET = controlflowvisualization
include(../Core/common_plugin.pri)
DEFINES += CONTROLFLOWVISUALIZATION_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -loointeraction
HEADERS += src/ControlFlowVisualizationException.h \
    src/allCFVisualizations.h \
    src/controlflowvisualization_api.h \
    src/items/ControlFlowItem.h \
    src/items/ControlFlowItemStyle.h \
    src/items/VBlockCF.h \
    src/items/VBlockCFStyle.h \
    src/items/VBreakStatementCF.h \
    src/items/VContinueStatementCF.h \
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
