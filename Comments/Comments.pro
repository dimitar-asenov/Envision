TARGET = comments
include(../Core/common_plugin.pri)
DEFINES += COMMENTS_LIBRARY
QT += webkit
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -lvisualizationbase \
    -linteractionbase \
    -lfilepersistence
HEADERS += src/handlers/HCommentDiagram.h \
    src/items/VCommentDiagramConnector.h \
    src/nodes/CommentDiagramConnector.h \
    src/nodes/CommentDiagramShape.h \
    src/items/VCommentBrowser.h \
    src/items/VCommentDiagramShape.h \
    src/items/VCommentDiagram.h \
    src/nodes/CommentDiagram.h \
    src/handlers/HComment.h \
    src/items/VComment.h \
    src/nodes/Comment.h \
    src/precompiled.h \
    src/CommentsException.h \
    src/comments_api.h \
    src/comments.h
SOURCES += src/handlers/HCommentDiagram.cpp \
    src/items/VCommentDiagramConnector.cpp \
    src/nodes/CommentDiagramConnector.cpp \
    src/nodes/CommentDiagramShape.cpp \
    src/items/VCommentBrowser.cpp \
    src/items/VCommentDiagramShape.cpp \
    src/items/VCommentDiagram.cpp \
    src/nodes/CommentDiagram.cpp \
    src/handlers/HComment.cpp \
    src/items/VComment.cpp \
    src/nodes/Comment.cpp \
    src/CommentsException.cpp \
    src/comments.cpp \
    test/SimpleTest.cpp
