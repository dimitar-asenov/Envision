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
HEADERS += headers/FormalResult.h \
    headers/allOOModelNodes.h \
    headers/Library.h \
    headers/TypedListInstantiations.h \
    headers/statements/AssignmentStatement.h \
    headers/expressions/ReferenceExpression.h \
    headers/expressions/MethodCallExpression.h \
    headers/statements/MethodCallStatement.h \
    headers/common/Static.h \
    headers/Field.h \
    headers/statements/ForEachStatement.h \
    headers/statements/SwitchCase.h \
    headers/statements/SwitchStatement.h \
    headers/statements/VariableDeclaration.h \
    headers/statements/ReturnStatement.h \
    headers/statements/ContinueStatement.h \
    headers/statements/BreakStatement.h \
    headers/FormalArgument.h \
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
    headers/MethodItem.h \
    headers/Method.h \
    headers/OOModelException.h \
    headers/common/Visibility.h \
    headers/common/attributeMacros.h \
    headers/Project.h \
    headers/Class.h \
    headers/Module.h \
    headers/oomodel_api.h \
    src/oomodel.h
SOURCES += src/FormalResult.cpp \
    src/Library.cpp \
    src/statements/AssignmentStatement.cpp \
    src/expressions/ReferenceExpression.cpp \
    src/expressions/MethodCallExpression.cpp \
    src/statements/MethodCallStatement.cpp \
    src/common/Static.cpp \
    src/Field.cpp \
    test/JavaTest.cpp \
    src/statements/ReturnStatement.cpp \
    src/statements/SwitchCase.cpp \
    src/statements/SwitchStatement.cpp \
    src/statements/VariableDeclaration.cpp \
    src/statements/ContinueStatement.cpp \
    src/statements/BreakStatement.cpp \
    src/FormalArgument.cpp \
    src/statements/ForAllStatement.cpp \
    src/statements/LoopStatement.cpp \
    src/statements/IfStatement.cpp \
    src/statements/Block.cpp \
    src/expressions/CastExpression.cpp \
    src/expressions/BinaryOperation.cpp \
    headers/expressions/UnaryOperation.cpp \
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
    src/MethodItem.cpp \
    src/Method.cpp \
    src/TypedListInstantiations.cpp \
    src/common/Visibility.cpp \
    src/OOModelException.cpp \
    src/Class.cpp \
    src/Project.cpp \
    src/Module.cpp \
    src/oomodel.cpp \
    test/SimpleTest.cpp
