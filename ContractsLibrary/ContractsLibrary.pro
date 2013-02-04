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
HEADERS += src/monitor/ValueAtReturnVisitor.h \
    src/monitor/ChangeMonitor.h \
    src/items/ContractsVMethodAddOn.h \
    src/interaction/CreateContractMethod.h \
    src/precompiled.h \
    src/ContractsLibraryException.h \
    src/contractslibrary_api.h \
    src/contractslibrary.h
SOURCES += src/monitor/ValueAtReturnVisitor.cpp \
    src/monitor/ChangeMonitor.cpp \
    src/items/ContractsVMethodAddOn.cpp \
    src/interaction/CreateContractMethod.cpp \
    src/ContractsLibraryException.cpp \
    src/contractslibrary.cpp \
    test/SimpleTest.cpp
