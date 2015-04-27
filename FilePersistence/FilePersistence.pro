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
    src/FilePersistencePlugin.h \
    src/version_control/ChangeDescription.h \
    src/version_control/Commit.h \
    src/version_control/CommitGraph.h \
    src/version_control/Diff.h \
    src/version_control/GitRepository.h \
    src/version_control/History.h \
    src/version_control/Merge.h
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
    src/FilePersistencePlugin.cpp \
    src/version_control/ChangeDescription.cpp \
    src/version_control/Diff.cpp \
    src/version_control/GitRepository.cpp \
    test/VersionControlDiffTests.cpp \
    src/version_control/CommitGraph.cpp \
    src/version_control/History.cpp \
    src/version_control/Commit.cpp \
    src/version_control/Merge.cpp
RESOURCES = FilePersistence.qrc

unix:LIBS += -lgit2
INCLUDEPATH += /usr/local/lib/libgit2-0.21.0/include/
