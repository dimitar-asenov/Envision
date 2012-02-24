ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror \
    -std=c++0x
INCLUDEPATH += ./headers \
    ./src \
    ./test \
    $${ENVISION_ROOT_DIR}
TARGET = oointeraction
DEFINES += OOINTERACTION_LIBRARY
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
PRECOMPILED_HEADER = headers/precompiled.h
HEADERS += headers/string_components/UnaryOperatorStringComponents.h \
    headers/expression_editor/operators/UnaryOperatorDescriptor.h \
    headers/handlers/SetCursorEvent.h \
    headers/string_providers/StaticStringProvider.h \
    headers/string_providers/TextRendererStringProvider.h \
    headers/string_providers/SequentialVisualizationStringProvider.h \
    headers/string_components/VariableAccessStringComponents.h \
    headers/string_components/UnfinishedOperatorStringComponents.h \
    headers/string_components/ErrorExpressionStringComponents.h \
    headers/string_components/BinaryOperatorStringComponents.h \
    headers/string_components/IntegerLiteralStringComponents.h \
    headers/string_components/EmptyExpressionStringComponents.h \
    headers/string_components/StringComponents.h \
    headers/string_providers/EmptyExpressionStringProvider.h \
    headers/string_providers/IntegerLiteralStringProvider.h \
    headers/string_providers/StringProvider.h \
    headers/handlers/HOOExpression.h \
    headers/expression_editor/OOOperatorDescriptorList.h \
    headers/expression_editor/operators/BinaryOperatorDescriptor.h \
    headers/expression_editor/OOOperatorDescriptor.h \
    headers/expression_editor/OOExpressionBuilder.h \
    headers/precompiled.h \
    headers/OOInteractionException.h \
    headers/oointeraction_api.h \
    src/oointeraction.h
SOURCES += src/string_components/UnaryOperatorStringComponents.cpp \
    src/expression_editor/operators/UnaryOperatorDescriptor.cpp \
    src/handlers/SetCursorEvent.cpp \
    src/string_providers/StaticStringProvider.cpp \
    src/string_providers/TextRendererStringProvider.cpp \
    src/string_providers/SequentialVisualizationStringProvider.cpp \
    src/string_components/BinaryOperatorStringComponents.cpp \
    src/string_components/EmptyExpressionStringComponents.cpp \
    src/string_components/ErrorExpressionStringComponents.cpp \
    src/string_components/IntegerLiteralStringComponents.cpp \
    src/string_components/UnfinishedOperatorStringComponents.cpp \
    src/string_components/VariableAccessStringComponents.cpp \
    src/string_components/StringComponents.cpp \
    src/string_providers/EmptyExpressionStringProvider.cpp \
    src/string_providers/IntegerLiteralStringProvider.cpp \
    src/string_providers/StringProvider.cpp \
    src/handlers/HOOExpression.cpp \
    src/expression_editor/OOOperatorDescriptorList.cpp \
    src/expression_editor/operators/BinaryOperatorDescriptor.cpp \
    src/expression_editor/OOOperatorDescriptor.cpp \
    src/expression_editor/OOExpressionBuilder.cpp \
    src/OOInteractionException.cpp \
    src/oointeraction.cpp \
    test/SimpleTest.cpp
