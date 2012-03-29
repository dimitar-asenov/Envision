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
TARGET = oovisualization
DEFINES += OOVISUALIZATION_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
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
HEADERS += src/expressions/VConditionalExpression.h \
    src/OOVisualizationException.h \
    src/allOOVisualizations.h \
    src/elements/VField.h \
    src/elements/VFieldStyle.h \
    src/elements/VFormalArgument.h \
    src/elements/VFormalArgumentStyle.h \
    src/elements/VFormalResult.h \
    src/elements/VFormalResultStyle.h \
    src/elements/VStorageSpecifier.h \
    src/elements/VVisibility.h \
    src/expressions/OperatorStyle.h \
    src/expressions/VArrayInitializer.h \
    src/expressions/VArrayInitializerStyle.h \
    src/expressions/VAssignmentExpression.h \
    src/expressions/VBinaryOperation.h \
    src/expressions/VCastExpression.h \
    src/expressions/VCastExpressionStyle.h \
    src/expressions/VCommaExpression.h \
    src/expressions/VEmptyExpression.h \
    src/expressions/VErrorExpression.h \
    src/expressions/VErrorExpressionStyle.h \
    src/expressions/VMethodCallExpression.h \
    src/expressions/VMethodCallExpressionStyle.h \
    src/expressions/VNewExpression.h \
    src/expressions/VNewExpressionStyle.h \
    src/expressions/VReferenceExpression.h \
    src/expressions/VReferenceExpressionStyle.h \
    src/expressions/VThisExpression.h \
    src/expressions/VUnaryOperation.h \
    src/expressions/VUnfinishedOperator.h \
    src/expressions/VUnfinishedOperatorStyle.h \
    src/expressions/VVariableAccess.h \
    src/expressions/VVariableAccessStyle.h \
    src/expressions/VVariableDeclaration.h \
    src/expressions/VVariableDeclarationStyle.h \
    src/literals/VBooleanLiteral.h \
    src/literals/VCharacterLiteral.h \
    src/literals/VFloatLiteral.h \
    src/literals/VIntegerLiteral.h \
    src/literals/VNullLiteral.h \
    src/literals/VStringLiteral.h \
    src/literals/VStringLiteralStyle.h \
    src/oovisualization_api.h \
    src/precompiled.h \
    src/statements/VBlock.h \
    src/statements/VBlockStyle.h \
    src/statements/VBreakStatement.h \
    src/statements/VContinueStatement.h \
    src/statements/VExpressionStatement.h \
    src/statements/VForEachStatement.h \
    src/statements/VForEachStatementStyle.h \
    src/statements/VIfStatement.h \
    src/statements/VIfStatementStyle.h \
    src/statements/VLoopStatement.h \
    src/statements/VLoopStatementStyle.h \
    src/statements/VReturnStatement.h \
    src/statements/VReturnStatementStyle.h \
    src/top_level/VClass.h \
    src/top_level/VClassStyle.h \
    src/top_level/VLibrary.h \
    src/top_level/VLibraryStyle.h \
    src/top_level/VMethod.h \
    src/top_level/VMethodStyle.h \
    src/top_level/VModule.h \
    src/top_level/VModuleStyle.h \
    src/top_level/VProject.h \
    src/top_level/VProjectStyle.h \
    src/types/VArrayType.h \
    src/types/VArrayTypeStyle.h \
    src/types/VNamedType.h \
    src/types/VPrimitiveType.h \
    src/oovisualization.h
SOURCES += src/expressions/VConditionalExpression.cpp \
    src/expressions/VVariableDeclarationStyle.cpp \
    src/expressions/VVariableDeclaration.cpp \
    src/expressions/VAssignmentExpression.cpp \
    src/expressions/VMethodCallExpressionStyle.cpp \
    src/statements/VExpressionStatement.cpp \
    src/expressions/VCommaExpression.cpp \
    src/expressions/VEmptyExpression.cpp \
    src/expressions/VUnfinishedOperatorStyle.cpp \
    src/expressions/VUnfinishedOperator.cpp \
    src/expressions/VErrorExpressionStyle.cpp \
    src/expressions/VErrorExpression.cpp \
    src/elements/VStorageSpecifier.cpp \
    src/expressions/VArrayInitializer.cpp \
    src/expressions/VArrayInitializerStyle.cpp \
    src/literals/VStringLiteralStyle.cpp \
    src/types/VArrayTypeStyle.cpp \
    src/types/VArrayType.cpp \
    src/statements/VContinueStatement.cpp \
    src/statements/VBreakStatement.cpp \
    src/statements/VForEachStatement.cpp \
    src/statements/VForEachStatementStyle.cpp \
    src/statements/VLoopStatement.cpp \
    src/statements/VLoopStatementStyle.cpp \
    src/elements/VField.cpp \
    src/elements/VFieldStyle.cpp \
    src/elements/VFormalArgument.cpp \
    src/elements/VFormalArgumentStyle.cpp \
    src/elements/VFormalResult.cpp \
    src/elements/VFormalResultStyle.cpp \
    src/elements/VVisibility.cpp \
    src/top_level/VClass.cpp \
    src/top_level/VClassStyle.cpp \
    src/top_level/VLibrary.cpp \
    src/top_level/VLibraryStyle.cpp \
    src/top_level/VMethod.cpp \
    src/top_level/VMethodStyle.cpp \
    src/top_level/VModule.cpp \
    src/top_level/VModuleStyle.cpp \
    src/top_level/VProject.cpp \
    src/top_level/VProjectStyle.cpp \
    src/statements/VIfStatement.cpp \
    src/statements/VIfStatementStyle.cpp \
    src/statements/VReturnStatement.cpp \
    src/statements/VReturnStatementStyle.cpp \
    src/statements/VBlock.cpp \
    src/statements/VBlockStyle.cpp \
    src/expressions/VBinaryOperation.cpp \
    src/expressions/VUnaryOperation.cpp \
    src/expressions/OperatorStyle.cpp \
    src/expressions/VNewExpression.cpp \
    src/expressions/VNewExpressionStyle.cpp \
    src/expressions/VCastExpression.cpp \
    src/expressions/VCastExpressionStyle.cpp \
    src/expressions/VThisExpression.cpp \
    src/expressions/VMethodCallExpression.cpp \
    src/literals/VNullLiteral.cpp \
    src/literals/VBooleanLiteral.cpp \
    src/literals/VCharacterLiteral.cpp \
    src/literals/VFloatLiteral.cpp \
    src/literals/VIntegerLiteral.cpp \
    src/types/VPrimitiveType.cpp \
    src/types/VNamedType.cpp \
    src/literals/VStringLiteral.cpp \
    src/expressions/VVariableAccessStyle.cpp \
    src/expressions/VVariableAccess.cpp \
    src/expressions/VReferenceExpression.cpp \
    src/expressions/VReferenceExpressionStyle.cpp \
    test/HelloWorldTest.cpp \
    src/OOVisualizationException.cpp \
    src/oovisualization.cpp
