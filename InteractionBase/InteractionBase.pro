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
PRECOMPILED_HEADER = src/precompiled.h
HEADERS += src/InteractionBaseException.h \
    src/commands/CSceneHandlerItemExit.h \
    src/commands/Command.h \
    src/commands/CommandError.h \
    src/commands/CommandExecutionEngine.h \
    src/commands/CommandHelp.h \
    src/commands/CommandResult.h \
    src/commands/CommandSuggestion.h \
    src/expression_editor/Empty.h \
    src/expression_editor/ErrorDescriptor.h \
    src/expression_editor/Expression.h \
    src/expression_editor/ExpressionContext.h \
    src/expression_editor/ExpressionEditor.h \
    src/expression_editor/ExpressionTreeUtils.h \
    src/expression_editor/ExpressionVisitor.h \
    src/expression_editor/Operator.h \
    src/expression_editor/OperatorDescriptor.h \
    src/expression_editor/OperatorDescriptorList.h \
    src/expression_editor/UnfinishedOperator.h \
    src/expression_editor/Value.h \
    src/expression_editor/parser/ParseResult.h \
    src/expression_editor/parser/Parser.h \
    src/expression_editor/parser/Token.h \
    src/expression_editor/tree_builder/AddEmptyValue.h \
    src/expression_editor/tree_builder/AddErrorOperator.h \
    src/expression_editor/tree_builder/AddOperator.h \
    src/expression_editor/tree_builder/AddValue.h \
    src/expression_editor/tree_builder/ExpressionTreeBuildInstruction.h \
    src/expression_editor/tree_builder/ExpressionTreeBuilder.h \
    src/expression_editor/tree_builder/FinishOperator.h \
    src/expression_editor/tree_builder/SkipOperatorDelimiter.h \
    src/handlers/GenericHandler.h \
    src/handlers/HCommandPrompt.h \
    src/handlers/HExtendable.h \
    src/handlers/HList.h \
    src/handlers/HPositionLayout.h \
    src/handlers/HSceneHandlerItem.h \
    src/handlers/HText.h \
    src/handlers/SetCursorEvent.h \
    src/interactionbase_api.h \
    src/precompiled.h \
    src/vis/CommandPrompt.h \
    src/vis/CommandPromptStyle.h \
    src/vis/TextAndDescription.h \
    src/vis/TextAndDescriptionStyle.h \
    src/interactionbase.h
SOURCES += src/handlers/SetCursorEvent.cpp \
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
