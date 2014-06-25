TARGET = comments
include(../Core/common_plugin.pri)
DEFINES += COMMENTS_LIBRARY
QT += webkitwidgets
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -lvisualizationbase \
    -linteractionbase \
    -lfilepersistence
HEADERS += src/handlers/HCommentImage.h \
    src/items/VCommentStyle.h \
    src/commands/CShapeSetProperty.h \
    src/commands/CCreateShape.h \
    src/handlers/HCommentDiagramConnector.h \
    src/handlers/HCommentDiagramShape.h \
    src/handlers/HComment.h \
    src/handlers/HCommentDiagram.h \
    src/nodes/CommentNode.h \
    src/nodes/CommentDiagram.h \
    src/nodes/CommentDiagramConnector.h \
    src/nodes/CommentDiagramShape.h \
    src/items/VComment.h \
    src/items/VCommentBrowser.h \
    src/items/VCommentDiagram.h \
    src/items/VCommentDiagramConnector.h \
    src/items/VCommentDiagramShape.h \
    src/items/VCommentDiagramShapeStyle.h \
    src/items/VCommentImage.h \
    src/precompiled.h \
    src/CommentsException.h \
    src/comments_api.h \
    src/comments.h \
    src/CommentDiagramToolbar.h \
    src/items/VCommentDiagramStyle.h \
    src/ColorPicker.h \
    src/OutlineTypePicker.h \
    src/items/VCommentFreeNode.h \
    src/items/VCommentTable.h \
    src/nodes/CommentFreeNode.h \
    src/nodes/CommentTable.h \
    src/nodes/CommentText.h \
    src/handlers/HCommentText.h \
    src/items/VCommentText.h \
    src/handlers/HCommentFreeNode.h
SOURCES += src/handlers/HCommentImage.cpp \
    src/items/VCommentStyle.cpp \
    src/commands/CShapeSetProperty.cpp \
    src/commands/CCreateShape.cpp \
    src/handlers/HCommentDiagramConnector.cpp \
    src/handlers/HCommentDiagramShape.cpp \
    src/handlers/HComment.cpp \
    src/handlers/HCommentDiagram.cpp \
    src/nodes/CommentNode.cpp \
    src/nodes/CommentDiagram.cpp \
    src/nodes/CommentDiagramConnector.cpp \
    src/nodes/CommentDiagramShape.cpp \
    src/items/VComment.cpp \
    src/items/VCommentBrowser.cpp \
    src/items/VCommentDiagram.cpp \
    src/items/VCommentDiagramConnector.cpp \
    src/items/VCommentDiagramShape.cpp \
    src/items/VCommentDiagramShapeStyle.cpp \
    src/items/VCommentImage.cpp \
    src/CommentsException.cpp \
    src/comments.cpp \
    test/SimpleTest.cpp \
    src/CommentDiagramToolbar.cpp \
    src/items/VCommentDiagramStyle.cpp \
    src/ColorPicker.cpp \
    src/OutlineTypePicker.cpp \
    src/items/VCommentFreeNode.cpp \
    src/items/VCommentTable.cpp \
    src/nodes/CommentFreeNode.cpp \
    src/nodes/CommentTable.cpp \
    src/nodes/CommentText.cpp \
    src/handlers/HCommentText.cpp \
    src/items/VCommentText.cpp \
    src/handlers/HCommentFreeNode.cpp

RESOURCES += \
    res.qrc
