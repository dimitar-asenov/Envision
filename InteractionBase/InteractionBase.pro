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
TARGET = interactionbase
DEFINES += INTERACTIONBASE_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest \
    -lmodelbase \
    -lfilepersistence \
    -lvisualizationbase
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
HEADERS += headers/handlers/HLayoutProvider.h \
    headers/expression_editor/ExpressionVisitor.h \
    headers/expression_editor/ExpressionEditor.h \
    headers/expression_editor/ExpressionTreeUtils.h \
    headers/expression_editor/parser/Parser.h \
    headers/expression_editor/parser/ParseResult.h \
    headers/expression_editor/parser/Token.h \
    headers/expression_editor/tree_builder/FinishOperator.h \
    headers/expression_editor/tree_builder/SkipOperatorDelimiter.h \
    headers/expression_editor/tree_builder/AddValue.h \
    headers/expression_editor/tree_builder/AddEmptyValue.h \
    headers/expression_editor/tree_builder/AddErrorOperator.h \
    headers/expression_editor/tree_builder/AddOperator.h \
    headers/expression_editor/tree_builder/ExpressionTreeBuildInstruction.h \
    headers/expression_editor/tree_builder/ExpressionTreeBuilder.h \
    headers/expression_editor/UnfinishedOperator.h \
    headers/expression_editor/ErrorDescriptor.h \
    headers/expression_editor/Empty.h \
    headers/expression_editor/ExpressionContext.h \
    headers/expression_editor/Value.h \
    headers/InteractionBaseException.h \
    headers/expression_editor/Operator.h \
    headers/expression_editor/Expression.h \
    headers/expression_editor/OperatorDescriptorList.h \
    headers/expression_editor/OperatorDescriptor.h \
    headers/precompiled.h \
    headers/handlers/HPositionLayout.h \
    headers/handlers/HList.h \
    headers/commands/CSceneHandlerItemExit.h \
    headers/handlers/HSceneHandlerItem.h \
    headers/handlers/GenericHandler.h \
    headers/handlers/HCommandPrompt.h \
    headers/handlers/HExtendable.h \
    headers/handlers/HText.h \
    headers/vis/CommandPromptStyle.h \
    headers/vis/CommandPrompt.h \
    headers/vis/TextAndDescriptionStyle.h \
    headers/vis/TextAndDescription.h \
    headers/commands/CommandExecutionEngine.h \
    headers/commands/CommandHelp.h \
    headers/commands/CommandError.h \
    headers/commands/CommandSuggestion.h \
    headers/commands/CommandResult.h \
    headers/commands/Command.h \
    headers/interactionbase_api.h \
    src/interactionbase.h
SOURCES += src/handlers/HLayoutProvider.cpp \
    src/expression_editor/ExpressionVisitor.cpp \
    src/expression_editor/ExpressionEditor.cpp \
    src/expression_editor/ExpressionTreeUtils.cpp \
    src/expression_editor/parser/Parser.cpp \
    src/expression_editor/parser/ParseResult.cpp \
    src/expression_editor/parser/Token.cpp \
    src/expression_editor/tree_builder/FinishOperator.cpp \
    src/expression_editor/tree_builder/SkipOperatorDelimiter.cpp \
    src/expression_editor/tree_builder/AddValue.cpp \
    src/expression_editor/tree_builder/AddEmptyValue.cpp \
    src/expression_editor/tree_builder/AddErrorOperator.cpp \
    src/expression_editor/tree_builder/AddOperator.cpp \
    src/expression_editor/tree_builder/ExpressionTreeBuildInstruction.cpp \
    src/expression_editor/tree_builder/ExpressionTreeBuilder.cpp \
    src/expression_editor/UnfinishedOperator.cpp \
    src/expression_editor/ErrorDescriptor.cpp \
    src/expression_editor/Empty.cpp \
    src/expression_editor/ExpressionContext.cpp \
    src/expression_editor/Value.cpp \
    src/InteractionBaseException.cpp \
    src/expression_editor/Operator.cpp \
    src/expression_editor/Expression.cpp \
    src/expression_editor/OperatorDescriptorList.cpp \
    src/expression_editor/OperatorDescriptor.cpp \
    src/handlers/HPositionLayout.cpp \
    src/handlers/HList.cpp \
    src/commands/CSceneHandlerItemExit.cpp \
    src/handlers/HSceneHandlerItem.cpp \
    src/handlers/GenericHandler.cpp \
    src/handlers/HCommandPrompt.cpp \
    src/handlers/HExtendable.cpp \
    src/handlers/HText.cpp \
    src/vis/CommandPromptStyle.cpp \
    src/vis/CommandPrompt.cpp \
    src/vis/TextAndDescriptionStyle.cpp \
    src/vis/TextAndDescription.cpp \
    src/commands/CommandExecutionEngine.cpp \
    src/commands/CommandHelp.cpp \
    src/commands/CommandError.cpp \
    src/commands/CommandSuggestion.cpp \
    src/commands/CommandResult.cpp \
    src/commands/Command.cpp \
    test/SimpleTest.cpp \
    src/interactionbase.cpp
