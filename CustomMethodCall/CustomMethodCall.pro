TARGET = custommethodcall
include(../Core/common_plugin.pri)
DEFINES += CUSTOMMETHODCALL_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization

HEADERS += src/CustomMethodCallException.h \
    src/custommethodcall_api.h \
    src/items/EmptyMethodVis.h \
    src/items/EmptyMethodVisStyle.h \
    src/items/FindMethodVis.h \
    src/items/FindMethodVisStyle.h \
    src/items/InsertMethodVis.h \
    src/items/InsertMethodVisStyle.h \
    src/items/SumMethodVis.h \
    src/items/SumMethodVisStyle.h \
    src/precompiled.h \
    src/custommethodcall.h
SOURCES += src/items/SumMethodVis.cpp \
    src/items/SumMethodVisStyle.cpp \
    src/items/InsertMethodVis.cpp \
    src/items/InsertMethodVisStyle.cpp \
    src/items/EmptyMethodVisStyle.cpp \
    src/items/EmptyMethodVis.cpp \
    src/items/FindMethodVis.cpp \
    src/items/FindMethodVisStyle.cpp \
    src/CustomMethodCallException.cpp \
    src/custommethodcall.cpp \
    test/SimpleTest.cpp
