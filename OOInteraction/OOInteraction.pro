TARGET = oointeraction
include(../Core/common_plugin.pri)
DEFINES += OOINTERACTION_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -lfilepersistence
HEADERS += src/expression_editor/CommandDescriptor.h \
    src/expression_editor/CompoundObjectDescriptor.h \
    src/expression_editor/commands/CommandExpression.h \
    src/expression_editor/commands/CreateMethodCall.h \
    src/string_offset_providers/GridConstructors.h \
    src/string_offset_providers/StringComponents.h \
    src/customization/CustomizationVisitor.h \
    src/commands/CCreateField.h \
    src/string_offset_providers/CompoundObjectStringOffsetProvider.h \
    src/commands/CSceneHandlerItemTest.h \
    src/handlers/HForEachStatement.h \
    src/handlers/HKeywordStatement.h \
    src/handlers/HReturnStatement.h \
    src/commands/CCreateProject.h \
    src/commands/CCreateMethod.h \
    src/commands/CCreateClass.h \
    src/string_offset_providers/Cell.h \
    src/string_offset_providers/ListCell.h \
    src/string_offset_providers/GridBasedOffsetProvider.h \
    src/OOInteractionException.h \
    src/expression_editor/OOExpressionBuilder.h \
    src/expression_editor/OOOperatorDescriptor.h \
    src/expression_editor/OOOperatorDescriptorList.h \
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
    src/string_offset_providers/EmptyExpressionStringOffsetProvider.h \
    src/string_offset_providers/InitializerStringOffsetProvider.h \
    src/string_offset_providers/StaticStringOffsetProvider.h \
    src/string_offset_providers/StringOffsetProvider.h \
    src/string_offset_providers/TextRendererStringOffsetProvider.h \
    src/oointeraction.h
SOURCES += src/expression_editor/CommandDescriptor.cpp \
    src/expression_editor/CompoundObjectDescriptor.cpp \
    src/expression_editor/commands/CreateMethodCall.cpp \
    src/string_offset_providers/GridConstructors.cpp \
    src/string_offset_providers/StringComponents.cpp \
    src/customization/CustomizationVisitor.cpp \
    src/commands/CCreateField.cpp \
    src/string_offset_providers/CompoundObjectStringOffsetProvider.cpp \
    src/commands/CSceneHandlerItemTest.cpp \
    src/handlers/HForEachStatement.cpp \
    src/handlers/HKeywordStatement.cpp \
    src/handlers/HReturnStatement.cpp \
    src/commands/CCreateProject.cpp \
    src/commands/CCreateMethod.cpp \
    src/commands/CCreateClass.cpp \
    src/string_offset_providers/Cell.cpp \
    src/string_offset_providers/ListCell.cpp \
    src/string_offset_providers/GridBasedOffsetProvider.cpp \
    src/handlers/HLoop.cpp \
    src/handlers/HIfStatement.cpp \
    src/string_offset_providers/TextRendererStringOffsetProvider.cpp \
    src/string_offset_providers/StringOffsetProvider.cpp \
    src/string_offset_providers/StaticStringOffsetProvider.cpp \
    src/string_offset_providers/InitializerStringOffsetProvider.cpp \
    src/string_offset_providers/EmptyExpressionStringOffsetProvider.cpp \
    src/handlers/HStatement.cpp \
    src/handlers/HFormalArgument.cpp \
    src/handlers/HMethod.cpp \
    src/handlers/HExpression.cpp \
    src/handlers/HClass.cpp \
    src/handlers/HProject.cpp \
    src/handlers/SetExpressionCursorEvent.cpp \
    src/expression_editor/OOOperatorDescriptorList.cpp \
    src/expression_editor/OOOperatorDescriptor.cpp \
    src/expression_editor/OOExpressionBuilder.cpp \
    src/OOInteractionException.cpp \
    src/oointeraction.cpp \
    test/SimpleTest.cpp
