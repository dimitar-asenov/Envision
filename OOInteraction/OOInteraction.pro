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
PRECOMPILED_HEADER = src/precompiled.h
HEADERS += src/string_components/ConditionalExpressionStringComponents.h \
    src/expression_editor/operators/ConditionalExpressionDescriptor.h \
    src/OOInteractionException.h \
    src/commands/CClassCreateMethod.h \
    src/commands/CProjectCreateClass.h \
    src/expression_editor/OOExpressionBuilder.h \
    src/expression_editor/OOOperatorDescriptor.h \
    src/expression_editor/OOOperatorDescriptorList.h \
    src/expression_editor/operators/ArrayTypeDescriptor.h \
    src/expression_editor/operators/AssignmentDescriptor.h \
    src/expression_editor/operators/BinaryOperatorDescriptor.h \
    src/expression_editor/operators/CallDescriptor.h \
    src/expression_editor/operators/CastDescriptor.h \
    src/expression_editor/operators/CommaDescriptor.h \
    src/expression_editor/operators/DeclarationDescriptor.h \
    src/expression_editor/operators/InitializerDescriptor.h \
    src/expression_editor/operators/MemberOperatorDescriptor.h \
    src/expression_editor/operators/NewArrayDescriptor.h \
    src/expression_editor/operators/UnaryOperatorDescriptor.h \
    src/handlers/HClass.h \
    src/handlers/HExpression.h \
    src/handlers/HFormalArgument.h \
    src/handlers/HIfStatement.h \
    src/handlers/HLoop.h \
    src/handlers/HMethod.h \
    src/handlers/HProject.h \
    src/handlers/HStatement.h \
    src/handlers/SetExpressionCursorEvent.h \
    src/oointeraction_api.h \
    src/precompiled.h \
    src/string_components/ArrayTypeStringComponents.h \
    src/string_components/AssignmentStringComponents.h \
    src/string_components/BinaryOperatorStringComponents.h \
    src/string_components/BooleanLiteralStringComponents.h \
    src/string_components/CallStringComponents.h \
    src/string_components/CastExpressionStringComponents.h \
    src/string_components/CommaExpressionStringComponents.h \
    src/string_components/EmptyExpressionStringComponents.h \
    src/string_components/ErrorExpressionStringComponents.h \
    src/string_components/InitializerStringComponents.h \
    src/string_components/IntegerLiteralStringComponents.h \
    src/string_components/ListStringComponents.h \
    src/string_components/NewArrayStringComponents.h \
    src/string_components/NullLiteralStringComponents.h \
    src/string_components/PrimitiveTypeStringComponents.h \
    src/string_components/StringComponents.h \
    src/string_components/StringLiteralStringComponents.h \
    src/string_components/ThisExpressionStringComponents.h \
    src/string_components/UnaryOperatorStringComponents.h \
    src/string_components/UnfinishedOperatorStringComponents.h \
    src/string_components/VariableAccessStringComponents.h \
    src/string_components/VariableDeclarationStringComponents.h \
    src/string_offset_providers/CallStringOffsetProvider.h \
    src/string_offset_providers/CastStringOffsetProvider.h \
    src/string_offset_providers/EmptyExpressionStringOffsetProvider.h \
    src/string_offset_providers/InitializerStringOffsetProvider.h \
    src/string_offset_providers/NewArrayStringOffsetProvider.h \
    src/string_offset_providers/SequentialVisualizationStringOffsetProvider.h \
    src/string_offset_providers/SimpleLiteralStringOffsetProvider.h \
    src/string_offset_providers/StaticStringOffsetProvider.h \
    src/string_offset_providers/StringOffsetProvider.h \
    src/string_offset_providers/TextRendererStringOffsetProvider.h \
    src/string_offset_providers/VariableDeclarationStringOffsetProvider.h \
    src/oointeraction.h
SOURCES += src/string_components/ConditionalExpressionStringComponents.cpp \
    src/expression_editor/operators/ConditionalExpressionDescriptor.cpp \
    src/handlers/HLoop.cpp \
    src/handlers/HIfStatement.cpp \
    src/string_offset_providers/VariableDeclarationStringOffsetProvider.cpp \
    src/string_components/VariableDeclarationStringComponents.cpp \
    src/expression_editor/operators/DeclarationDescriptor.cpp \
    src/string_offset_providers/CastStringOffsetProvider.cpp \
    src/string_components/ListStringComponents.cpp \
    src/string_offset_providers/TextRendererStringOffsetProvider.cpp \
    src/string_offset_providers/StringOffsetProvider.cpp \
    src/string_offset_providers/StaticStringOffsetProvider.cpp \
    src/string_offset_providers/SimpleLiteralStringOffsetProvider.cpp \
    src/string_offset_providers/SequentialVisualizationStringOffsetProvider.cpp \
    src/string_offset_providers/NewArrayStringOffsetProvider.cpp \
    src/string_offset_providers/InitializerStringOffsetProvider.cpp \
    src/string_offset_providers/EmptyExpressionStringOffsetProvider.cpp \
    src/string_offset_providers/CallStringOffsetProvider.cpp \
    src/string_components/AssignmentStringComponents.cpp \
    src/expression_editor/operators/AssignmentDescriptor.cpp \
    src/handlers/HStatement.cpp \
    src/handlers/HFormalArgument.cpp \
    src/string_components/ArrayTypeStringComponents.cpp \
    src/expression_editor/operators/ArrayTypeDescriptor.cpp \
    src/string_components/PrimitiveTypeStringComponents.cpp \
    src/string_components/StringLiteralStringComponents.cpp \
    src/commands/CProjectCreateClass.cpp \
    src/handlers/HMethod.cpp \
    src/handlers/HExpression.cpp \
    src/handlers/HClass.cpp \
    src/handlers/HProject.cpp \
    src/handlers/SetExpressionCursorEvent.cpp \
    src/commands/CClassCreateMethod.cpp \
    src/string_components/CallStringComponents.cpp \
    src/expression_editor/operators/CallDescriptor.cpp \
    src/expression_editor/operators/MemberOperatorDescriptor.cpp \
    src/string_components/NewArrayStringComponents.cpp \
    src/string_components/InitializerStringComponents.cpp \
    src/string_components/CommaExpressionStringComponents.cpp \
    src/string_components/CastExpressionStringComponents.cpp \
    src/expression_editor/operators/CommaDescriptor.cpp \
    src/expression_editor/operators/CastDescriptor.cpp \
    src/expression_editor/operators/InitializerDescriptor.cpp \
    src/expression_editor/operators/NewArrayDescriptor.cpp \
    src/string_components/ThisExpressionStringComponents.cpp \
    src/string_components/NullLiteralStringComponents.cpp \
    src/string_components/BooleanLiteralStringComponents.cpp \
    src/string_components/UnaryOperatorStringComponents.cpp \
    src/expression_editor/operators/UnaryOperatorDescriptor.cpp \
    src/string_components/BinaryOperatorStringComponents.cpp \
    src/string_components/EmptyExpressionStringComponents.cpp \
    src/string_components/ErrorExpressionStringComponents.cpp \
    src/string_components/IntegerLiteralStringComponents.cpp \
    src/string_components/UnfinishedOperatorStringComponents.cpp \
    src/string_components/VariableAccessStringComponents.cpp \
    src/string_components/StringComponents.cpp \
    src/expression_editor/OOOperatorDescriptorList.cpp \
    src/expression_editor/operators/BinaryOperatorDescriptor.cpp \
    src/expression_editor/OOOperatorDescriptor.cpp \
    src/expression_editor/OOExpressionBuilder.cpp \
    src/OOInteractionException.cpp \
    src/oointeraction.cpp \
    test/SimpleTest.cpp
