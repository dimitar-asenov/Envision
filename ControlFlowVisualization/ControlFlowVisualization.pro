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
INSTALLS += target \
    pluginmeta
HEADERS += headers/allCFVisualizations.h \
    headers/items/VMethodCFStyle.h \
    headers/items/VMethodCF.h \
    headers/items/ControlFlowItemStyle.h \
    headers/items/ControlFlowItem.h \
    headers/ControlFlowVisualizationException.h \
    headers/controlflowvisualization_api.h \
    src/controlflowvisualization.h
SOURCES += src/items/VMethodCF.cpp \
    src/items/VMethodCFStyle.cpp \
    src/items/ControlFlowItem.cpp \
    src/items/ControlFlowItemStyle.cpp \
    src/ControlFlowVisualizationException.cpp \
    src/controlflowvisualization.cpp \
    test/SimpleTest.cpp
