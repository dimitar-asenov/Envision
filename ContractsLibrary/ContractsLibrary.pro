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
HEADERS += src/items/VContractCall.h \
    src/items/SimpleKeywordCallStyle.h \
    src/precompiled.h \
    src/ContractsLibraryException.h \
    src/contractslibrary_api.h \
    src/contractslibrary.h
SOURCES += src/items/VContractCall.cpp \
    src/items/SimpleKeywordCallStyle.cpp \
    src/ContractsLibraryException.cpp \
    src/contractslibrary.cpp \
    test/SimpleTest.cpp
