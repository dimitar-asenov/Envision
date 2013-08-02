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
HEADERS += src/handlers/HComment.h \
    src/items/VComment.h \
    src/nodes/Comment.h \
    src/items/MarkdownTextItem.h \
    src/precompiled.h \
    src/CommentsException.h \
    src/comments_api.h \
    src/comments.h
SOURCES += src/handlers/HComment.cpp \
    src/items/VComment.cpp \
    src/nodes/Comment.cpp \
    src/items/MarkdownTextItem.cpp \
    src/CommentsException.cpp \
    src/comments.cpp \
    test/SimpleTest.cpp
