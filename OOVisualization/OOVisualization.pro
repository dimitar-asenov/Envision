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
    thread
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
styles.path = $${BUILD_DIR}/styles
styles.files = styles/*
INSTALLS += target \
    pluginmeta \
    styles
HEADERS += headers/literals/VStringLiteralStyle.h \
    headers/types/VArrayTypeStyle.h \
    headers/types/VArrayType.h \
    headers/statements/VContinueStatement.h \
    headers/statements/VBreakStatement.h \
    headers/statements/VForEachStatementStyle.h \
    headers/statements/VForEachStatement.h \
    headers/statements/VLoopStatementStyle.h \
    headers/statements/VLoopStatement.h \
    headers/elements/VField.h \
    headers/elements/VFieldStyle.h \
    headers/elements/VFormalArgument.h \
    headers/elements/VFormalArgumentStyle.h \
    headers/elements/VFormalResult.h \
    headers/elements/VFormalResultStyle.h \
    headers/elements/VStatic.h \
    headers/elements/VVisibility.h \
    headers/top_level/VClass.h \
    headers/top_level/VClassStyle.h \
    headers/top_level/VLibrary.h \
    headers/top_level/VLibraryStyle.h \
    headers/top_level/VMethod.h \
    headers/top_level/VMethodStyle.h \
    headers/top_level/VModule.h \
    headers/top_level/VModuleStyle.h \
    headers/top_level/VProject.h \
    headers/top_level/VProjectStyle.h \
    headers/statements/VIfStatementStyle.h \
    headers/statements/VIfStatement.h \
    headers/statements/VReturnStatementStyle.h \
    headers/statements/VReturnStatement.h \
    headers/statements/VBlockStyle.h \
    headers/statements/VBlock.h \
    headers/statements/VAssignmentStatement.h \
    headers/expressions/VBinaryOperation.h \
    headers/expressions/VUnaryOperation.h \
    headers/expressions/OperatorStyle.h \
    headers/expressions/VNewExpressionStyle.h \
    headers/expressions/VNewExpression.h \
    headers/expressions/VCastExpressionStyle.h \
    headers/expressions/VCastExpression.h \
    headers/expressions/VThisExpression.h \
    headers/expressions/VMethodCallExpressionStyle.h \
    headers/expressions/VMethodCallExpression.h \
    headers/literals/VNullLiteral.h \
    headers/statements/VVariableDeclarationStyle.h \
    headers/statements/VVariableDeclaration.h \
    headers/literals/VBooleanLiteral.h \
    headers/literals/VCharacterLiteral.h \
    headers/literals/VFloatLiteral.h \
    headers/literals/VIntegerLiteral.h \
    headers/allOOVisualizations.h \
    headers/types/VPrimitiveType.h \
    headers/types/VNamedType.h \
    headers/literals/VStringLiteral.h \
    headers/statements/VMethodCallStatementStyle.h \
    headers/statements/VMethodCallStatement.h \
    headers/expressions/VVariableAccessStyle.h \
    headers/expressions/VVariableAccess.h \
    headers/expressions/VReferenceExpressionStyle.h \
    headers/expressions/VReferenceExpression.h \
    headers/icons/ClassIconStyle.h \
    headers/icons/ClassIcon.h \
    headers/OOVisualizationException.h \
    headers/oovisualization_api.h \
    src/oovisualization.h
SOURCES += src/literals/VStringLiteralStyle.cpp \
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
    src/elements/VStatic.cpp \
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
    src/statements/VAssignmentStatement.cpp \
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
    src/statements/VVariableDeclaration.cpp \
    src/statements/VVariableDeclarationStyle.cpp \
    src/literals/VBooleanLiteral.cpp \
    src/literals/VCharacterLiteral.cpp \
    src/literals/VFloatLiteral.cpp \
    src/literals/VIntegerLiteral.cpp \
    src/types/VPrimitiveType.cpp \
    src/types/VNamedType.cpp \
    src/literals/VStringLiteral.cpp \
    src/statements/VMethodCallStatement.cpp \
    src/statements/VMethodCallStatementStyle.cpp \
    src/expressions/VVariableAccessStyle.cpp \
    src/expressions/VVariableAccess.cpp \
    src/expressions/VReferenceExpression.cpp \
    src/expressions/VReferenceExpressionStyle.cpp \
    src/icons/ClassIcon.cpp \
    src/icons/ClassIconStyle.cpp \
    test/HelloWorldTest.cpp \
    src/OOVisualizationException.cpp \
    src/oovisualization.cpp
