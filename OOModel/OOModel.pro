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
    thread
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target \
    pluginmeta
HEADERS += headers/elements/FormalArgument.h \
    headers/elements/FormalResult.h \
    headers/elements/StatementItem.h \
    headers/top_level/Class.h \
    headers/top_level/Field.h \
    headers/top_level/Library.h \
    headers/top_level/Method.h \
    headers/top_level/Module.h \
    headers/top_level/Project.h \
    headers/types/ArrayType.h \
    headers/allOOModelNodes.h \
    headers/TypedListInstantiations.h \
    headers/statements/AssignmentStatement.h \
    headers/expressions/ReferenceExpression.h \
    headers/expressions/MethodCallExpression.h \
    headers/statements/MethodCallStatement.h \
    headers/common/Static.h \
    headers/statements/ForEachStatement.h \
    headers/statements/SwitchCase.h \
    headers/statements/SwitchStatement.h \
    headers/statements/VariableDeclaration.h \
    headers/statements/ReturnStatement.h \
    headers/statements/ContinueStatement.h \
    headers/statements/BreakStatement.h \
    headers/statements/LoopStatement.h \
    headers/statements/IfStatement.h \
    headers/statements/Block.h \
    headers/expressions/CastExpression.h \
    headers/expressions/BinaryOperation.h \
    headers/expressions/UnaryOperation.h \
    headers/types/NamedType.h \
    headers/types/PrimitiveType.h \
    headers/types/Type.h \
    headers/expressions/NewExpression.h \
    headers/expressions/VariableAccess.h \
    headers/expressions/ThisExpression.h \
    headers/expressions/NullLiteral.h \
    headers/expressions/CharacterLiteral.h \
    headers/expressions/BooleanLiteral.h \
    headers/expressions/StringLiteral.h \
    headers/expressions/FloatLiteral.h \
    headers/expressions/IntegerLiteral.h \
    headers/statements/Statement.h \
    headers/expressions/Expression.h \
    headers/OOModelException.h \
    headers/common/Visibility.h \
    headers/common/attributeMacros.h \
    headers/oomodel_api.h \
    src/oomodel.h
SOURCES += src/elements/FormalArgument.cpp \
    src/elements/FormalResult.cpp \
    src/elements/StatementItem.cpp \
    src/top_level/Class.cpp \
    src/top_level/Field.cpp \
    src/top_level/Library.cpp \
    src/top_level/Method.cpp \
    src/top_level/Module.cpp \
    src/top_level/Project.cpp \
    src/types/ArrayType.cpp \
    src/statements/ForEachStatement.cpp \
    src/expressions/UnaryOperation.cpp \
    src/statements/AssignmentStatement.cpp \
    src/expressions/ReferenceExpression.cpp \
    src/expressions/MethodCallExpression.cpp \
    src/statements/MethodCallStatement.cpp \
    src/common/Static.cpp \
    test/JavaTest.cpp \
    src/statements/ReturnStatement.cpp \
    src/statements/SwitchCase.cpp \
    src/statements/SwitchStatement.cpp \
    src/statements/VariableDeclaration.cpp \
    src/statements/ContinueStatement.cpp \
    src/statements/BreakStatement.cpp \
    src/statements/LoopStatement.cpp \
    src/statements/IfStatement.cpp \
    src/statements/Block.cpp \
    src/expressions/CastExpression.cpp \
    src/expressions/BinaryOperation.cpp \
    src/types/NamedType.cpp \
    src/types/PrimitiveType.cpp \
    src/types/Type.cpp \
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
    src/common/Visibility.cpp \
    src/OOModelException.cpp \
    src/oomodel.cpp \
    test/SimpleTest.cpp
