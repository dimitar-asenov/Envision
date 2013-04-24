TARGET = comments
include(../Core/common_plugin.pri)

DEFINES += COMMENTS_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -lvisualizationbase \
    -linteractionbase \
    -lfilepersistence

HEADERS += src/precompiled.h \
    src/CommentsException.h \
	src/comments_api.h \
    src/comments.h
SOURCES += src/CommentsException.cpp \
	src/comments.cpp \
	test/SimpleTest.cpp
