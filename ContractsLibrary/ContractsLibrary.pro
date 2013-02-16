TARGET = contractslibrary
include(../Core/common_plugin.pri)

TEMPLATE=lib

CONFIG+=debug_and_release

DEFINES += CONTRACTSLIBRARY_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -loointeraction
HEADERS += src/items/ContractFilter.h \
    src/items/SignatureContractsVMethodAddOn.h \
    src/items/InterfaceContractsVMethodAddOn.h \
    src/monitor/ValueAtReturnVisitor.h \
    src/monitor/ChangeMonitor.h \
    src/precompiled.h \
    src/ContractsLibraryException.h \
    src/contractslibrary_api.h \
    src/contractslibrary.h
SOURCES += src/items/ContractFilter.cpp \
    src/items/SignatureContractsVMethodAddOn.cpp \
    src/items/InterfaceContractsVMethodAddOn.cpp \
    src/monitor/ValueAtReturnVisitor.cpp \
    src/monitor/ChangeMonitor.cpp \
    src/ContractsLibraryException.cpp \
    src/contractslibrary.cpp \
    test/SimpleTest.cpp
