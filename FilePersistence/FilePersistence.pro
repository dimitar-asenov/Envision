TARGET = filepersistence
include(../Core/common_plugin.pri)
DEFINES += FILEPERSISTENCE_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase
QT += xml
HEADERS += src/simple/GenericNode.h \
    src/simple/GenericNodeAllocator.h \
    src/simple/SimpleTextFileStore.h \
    src/FilePersistenceException.h \
    src/FileStore.h \
    src/SystemClipboard.h \
    src/XMLModel.h \
    src/filepersistence_api.h \
    src/precompiled.h \
    src/FilePersistencePlugin.h
SOURCES += src/simple/GenericNode.cpp \
    src/simple/GenericNodeAllocator.cpp \
    src/simple/SimpleTextFileStore.cpp \
    test/TypedListTests.cpp \
    test/ClipboardTests.cpp \
    src/XMLModel.cpp \
    src/SystemClipboard.cpp \
    test/LoadNodeDataTests.cpp \
    test/PartialNodeTests.cpp \
    src/FilePersistenceException.cpp \
    test/LoadTests.cpp \
    test/SaveTests.cpp \
    src/FileStore.cpp \
    src/FilePersistencePlugin.cpp
RESOURCES = FilePersistence.qrc
