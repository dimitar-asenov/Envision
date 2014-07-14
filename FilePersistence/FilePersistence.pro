TARGET = filepersistence
include(../Core/common_plugin.pri)
DEFINES += FILEPERSISTENCE_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase
QT += xml
HEADERS += src/simple/Parser.h \
    src/simple/GenericNode.h \
    src/simple/GenericNodeAllocator.h \
    src/simple/SimpleTextFileStore.h \
    src/FilePersistenceException.h \
    src/SystemClipboard.h \
    src/XMLModel.h \
    src/filepersistence_api.h \
    src/precompiled.h \
    src/FilePersistencePlugin.h \
    src/version_control/ChangeDescription.h \
    src/version_control/Diff.h \
    src/version_control/GitRepository.h \
    src/version_control/CommitGraph.h
SOURCES += src/simple/Parser.cpp \
    src/simple/GenericNode.cpp \
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
    src/FilePersistencePlugin.cpp \
    src/version_control/ChangeDescription.cpp \
    src/version_control/Diff.cpp \
    src/version_control/GitRepository.cpp \
    test/VersionControlDiffTests.cpp \
    src/version_control/CommitGraph.cpp
RESOURCES = FilePersistence.qrc

# Fix this in a way that library is included in project
unix:LIBS += -lgit2
INCLUDEPATH += /usr/local/lib/libgit2-0.21.0/include/
