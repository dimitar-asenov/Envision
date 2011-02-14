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
HEADERS += headers/expressions/VThisExpression.h \
    headers/expressions/VMethodCallExpressionStyle.h \
    headers/expressions/VMethodCallExpression.h \
    headers/literals/VNullLiteral.h \
    headers/statements/VVariableDeclarationStyle.h \
    headers/statements/VVariableDeclaration.h \
    headers/literals/VBooleanLiteral.h \
    headers/literals/VCharacterLiteral.h \
    headers/literals/VFloatLiteral.h \
    headers/literals/VIntegerLiteral.h \
    headers/vis/elements/VFormalResultStyle.h \
    headers/vis/elements/VFormalResult.h \
    headers/allOOVisualizations.h \
    headers/types/VPrimitiveType.h \
    headers/vis/top_level/VLibraryStyle.h \
    headers/vis/top_level/VLibrary.h \
    headers/vis/elements/VFormalArgumentStyle.h \
    headers/vis/elements/VFormalArgument.h \
    headers/types/VNamedType.h \
    headers/literals/VStringLiteral.h \
    headers/statements/VMethodCallStatementStyle.h \
    headers/statements/VMethodCallStatement.h \
    headers/expressions/VVariableAccessStyle.h \
    headers/expressions/VVariableAccess.h \
    headers/expressions/VReferenceExpressionStyle.h \
    headers/expressions/VReferenceExpression.h \
    headers/icons/MethodIconStyle.h \
    headers/icons/MethodIcon.h \
    headers/icons/ClassIconStyle.h \
    headers/icons/ClassIcon.h \
    headers/vis/elements/VFieldStyle.h \
    headers/vis/elements/VField.h \
    headers/vis/top_level/VClassStyle.h \
    headers/vis/top_level/VClass.h \
    headers/vis/top_level/VModuleStyle.h \
    headers/vis/top_level/VModule.h \
    headers/vis/top_level/VProjectStyle.h \
    headers/vis/top_level/VProject.h \
    headers/vis/VStatic.h \
    headers/vis/VVisibility.h \
    headers/OOVisualizationException.h \
    headers/oovisualization_api.h \
    src/oovisualization.h
SOURCES += src/expressions/VThisExpression.cpp \
    src/expressions/VMethodCallExpression.cpp \
    src/literals/VNullLiteral.cpp \
    src/statements/VVariableDeclaration.cpp \
    src/statements/VVariableDeclarationStyle.cpp \
    src/literals/VBooleanLiteral.cpp \
    src/literals/VCharacterLiteral.cpp \
    src/literals/VFloatLiteral.cpp \
    src/literals/VIntegerLiteral.cpp \
    src/vis/elements/VFormalResult.cpp \
    src/vis/elements/VFormalResultStyle.cpp \
    src/types/VPrimitiveType.cpp \
    src/vis/top_level/VLibrary.cpp \
    src/vis/top_level/VLibraryStyle.cpp \
    src/vis/elements/VFormalArgument.cpp \
    src/vis/elements/VFormalArgumentStyle.cpp \
    src/types/VNamedType.cpp \
    src/literals/VStringLiteral.cpp \
    src/statements/VMethodCallStatement.cpp \
    src/statements/VMethodCallStatementStyle.cpp \
    src/expressions/VVariableAccessStyle.cpp \
    src/expressions/VVariableAccess.cpp \
    src/expressions/VReferenceExpression.cpp \
    src/expressions/VReferenceExpressionStyle.cpp \
    src/icons/MethodIcon.cpp \
    src/icons/MethodIconStyle.cpp \
    src/icons/ClassIcon.cpp \
    src/icons/ClassIconStyle.cpp \
    src/vis/elements/VFieldStyle.cpp \
    src/vis/elements/VField.cpp \
    src/vis/top_level/VMethodStyle.cpp \
    src/vis/top_level/VMethod.cpp \
    src/vis/top_level/VClass.cpp \
    src/vis/top_level/VClassStyle.cpp \
    src/vis/top_level/VModule.cpp \
    src/vis/top_level/VModuleStyle.cpp \
    src/vis/top_level/VProjectStyle.cpp \
    src/vis/top_level/VProject.cpp \
    src/vis/VStatic.cpp \
    src/vis/VVisibility.cpp \
    test/HelloWorldTest.cpp \
    src/OOVisualizationException.cpp \
    src/oovisualization.cpp
