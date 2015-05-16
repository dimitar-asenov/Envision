TARGET = filepersistence
include(../Core/common_plugin.pri)
QMAKE_RESOURCE_FLAGS += -no-compress
DEFINES += FILEPERSISTENCE_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase
QT += xml
HEADERS += src/simple/GenericPersistentUnit.h \
    src/simple/GenericTree.h \
    src/simple/Parser.h \
    src/simple/GenericNode.h \
    src/simple/SimpleTextFileStore.h \
    src/FilePersistenceException.h \
    src/SystemClipboard.h \
    src/XMLModel.h \
    src/filepersistence_api.h \
    src/precompiled.h \
    src/FilePersistencePlugin.h
SOURCES += src/simple/GenericPersistentUnit.cpp \
    src/simple/GenericTree.cpp \
    src/simple/Parser.cpp \
    src/simple/GenericNode.cpp \
    src/simple/SimpleTextFileStore.cpp \
    test/TypedListTests.cpp \
    test/ClipboardTests.cpp \
    src/XMLModel.cpp \
    src/SystemClipboard.cpp \
    test/PartialNodeTests.cpp \
    src/FilePersistenceException.cpp \
    test/LoadTests.cpp \
    test/SaveTests.cpp \
    src/FilePersistencePlugin.cpp
RESOURCES = FilePersistence.qrc
