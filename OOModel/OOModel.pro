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
TARGET = oomodel
DEFINES += OOMODEL_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest \
    -lmodelbase
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
INSTALLS += target \
    pluginmeta
PRECOMPILED_HEADER = src/precompiled.h
HEADERS += src/expressions/types/ArrayTypeExpression.h \
    src/expressions/types/ClassTypeExpression.h \
    src/expressions/types/PrimitiveTypeExpression.h \
    src/expressions/types/TypeExpression.h \
    src/elements/OOReference.h \
    src/expressions/ConditionalExpression.h \
    src/OOModelException.h \
    src/TypedListInstantiations.h \
    src/allOOModelNodes.h \
    src/attributeMacros.h \
    src/elements/FormalArgument.h \
    src/elements/FormalResult.h \
    src/elements/StatementItem.h \
    src/elements/StatementItemList.h \
    src/elements/StorageSpecifier.h \
    src/elements/Visibility.h \
    src/expressions/ArrayInitializer.h \
    src/expressions/AssignmentExpression.h \
    src/expressions/BinaryOperation.h \
    src/expressions/BooleanLiteral.h \
    src/expressions/CastExpression.h \
    src/expressions/CharacterLiteral.h \
    src/expressions/CommaExpression.h \
    src/expressions/EmptyExpression.h \
    src/expressions/ErrorExpression.h \
    src/expressions/Expression.h \
    src/expressions/FloatLiteral.h \
    src/expressions/IntegerLiteral.h \
    src/expressions/MethodCallExpression.h \
    src/expressions/NewExpression.h \
    src/expressions/NullLiteral.h \
    src/expressions/ReferenceExpression.h \
    src/expressions/StringLiteral.h \
    src/expressions/ThisExpression.h \
    src/expressions/UnaryOperation.h \
    src/expressions/UnfinishedOperator.h \
    src/expressions/VariableAccess.h \
    src/expressions/VariableDeclaration.h \
    src/oomodel_api.h \
    src/precompiled.h \
    src/statements/Block.h \
    src/statements/BreakStatement.h \
    src/statements/ContinueStatement.h \
    src/statements/ExpressionStatement.h \
    src/statements/ForEachStatement.h \
    src/statements/IfStatement.h \
    src/statements/LoopStatement.h \
    src/statements/ReturnStatement.h \
    src/statements/Statement.h \
    src/statements/SwitchCase.h \
    src/statements/SwitchStatement.h \
    src/top_level/Class.h \
    src/top_level/Field.h \
    src/top_level/Library.h \
    src/top_level/Method.h \
    src/top_level/Module.h \
    src/top_level/Project.h \
    src/oomodel.h
SOURCES += src/expressions/types/ArrayTypeExpression.cpp \
    src/expressions/types/ClassTypeExpression.cpp \
    src/expressions/types/PrimitiveTypeExpression.cpp \
    src/expressions/types/TypeExpression.cpp \
    src/elements/OOReference.cpp \
    src/expressions/ConditionalExpression.cpp \
    src/expressions/VariableDeclaration.cpp \
    src/expressions/AssignmentExpression.cpp \
    src/statements/ExpressionStatement.cpp \
    src/expressions/CommaExpression.cpp \
    src/expressions/UnfinishedOperator.cpp \
    src/expressions/ErrorExpression.cpp \
    src/expressions/EmptyExpression.cpp \
    src/elements/StorageSpecifier.cpp \
    src/elements/StatementItemList.cpp \
    src/expressions/ArrayInitializer.cpp \
    src/elements/Visibility.cpp \
    src/elements/FormalArgument.cpp \
    src/elements/FormalResult.cpp \
    src/elements/StatementItem.cpp \
    src/top_level/Class.cpp \
    src/top_level/Field.cpp \
    src/top_level/Library.cpp \
    src/top_level/Method.cpp \
    src/top_level/Module.cpp \
    src/top_level/Project.cpp \
    src/statements/ForEachStatement.cpp \
    src/expressions/UnaryOperation.cpp \
    src/expressions/ReferenceExpression.cpp \
    src/expressions/MethodCallExpression.cpp \
    test/JavaTest.cpp \
    src/statements/ReturnStatement.cpp \
    src/statements/SwitchCase.cpp \
    src/statements/SwitchStatement.cpp \
    src/statements/ContinueStatement.cpp \
    src/statements/BreakStatement.cpp \
    src/statements/LoopStatement.cpp \
    src/statements/IfStatement.cpp \
    src/statements/Block.cpp \
    src/expressions/CastExpression.cpp \
    src/expressions/BinaryOperation.cpp \
    src/expressions/NewExpression.cpp \
    src/expressions/VariableAccess.cpp \
    src/expressions/ThisExpression.cpp \
    src/expressions/NullLiteral.cpp \
    src/expressions/CharacterLiteral.cpp \
    src/expressions/BooleanLiteral.cpp \
    src/expressions/StringLiteral.cpp \
    src/expressions/FloatLiteral.cpp \
    src/expressions/IntegerLiteral.cpp \
    src/statements/Statement.cpp \
    src/expressions/Expression.cpp \
    src/TypedListInstantiations.cpp \
    src/OOModelException.cpp \
    src/oomodel.cpp \
    test/SimpleTest.cpp
