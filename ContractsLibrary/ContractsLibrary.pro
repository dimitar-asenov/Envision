TARGET = contractslibrary
include(../Core/common_plugin.pri)
DEFINES += CONTRACTSLIBRARY_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -loointeraction
HEADERS += src/items/SimpleKeywordCallStyle.h \
    src/items/VRequiresCall.h \
    src/precompiled.h \
    src/ContractsLibraryException.h \
    src/contractslibrary_api.h \
    src/contractslibrary.h
SOURCES += src/items/SimpleKeywordCallStyle.cpp \
    src/items/VRequiresCall.cpp \
    src/ContractsLibraryException.cpp \
    src/contractslibrary.cpp \
    test/SimpleTest.cpp
