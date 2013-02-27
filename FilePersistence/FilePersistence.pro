TARGET = filepersistence
include(../Core/common_plugin.pri)
DEFINES += FILEPERSISTENCE_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase
QT += xml

HEADERS += src/FilePersistenceException.h \
    src/FileStore.h \
    src/NodeIdMap.h \
    src/SystemClipboard.h \
    src/XMLModel.h \
    src/filepersistence_api.h \
    src/precompiled.h \
    src/filepersistence.h
SOURCES += src/NodeIdMap.cpp \
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
    src/filepersistence.cpp
RESOURCES = FilePersistence.qrc
