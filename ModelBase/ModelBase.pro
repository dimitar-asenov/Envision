ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror
INCLUDEPATH += ./headers \
    ./src \
    ./test \
    $${ENVISION_ROOT_DIR}/Core/headers \
    $${ENVISION_ROOT_DIR}
TARGET = modelbase
DEFINES += MODELBASE_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest
QT = core \
    gui
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target \
    pluginmeta
HEADERS += headers/test_nodes/BinaryNodeAccessUnit.h \
    headers/test_nodes/BinaryNode.h \
    headers/test_nodes/BinaryWithPosition.h \
    headers/commands/ListRemove.h \
    headers/commands/ListInsert.h \
    headers/commands/NameChange.h \
    headers/commands/PointerFieldSet.h \
    headers/nodes/List.h \
    test/NotificationListener.h \
    headers/commands/UndoCommand.h \
    headers/nodes/nodeMacros.h \
    headers/nodes/Node.h \
    headers/nodes/Extendable/Attribute.h \
    headers/nodes/Extendable/AttributeChain.h \
    headers/nodes/Extendable/ExtendableIndex.h \
    headers/nodes/Extendable/ExtendableNode.h \
    src/commands/SetModificationTarget.h \
    headers/InterruptibleThread.h \
    headers/NodeReadWriteLock.h \
    headers/commands/FieldSet.h \
    headers/nodes/Reference.h \
    test/PersistentStoreMock.h \
    headers/ModelException.h \
    headers/nodes/Integer.h \
    headers/nodes/Text.h \
    headers/commands/ExtendedNodeOptional.h \
    headers/Model.h \
    headers/PersistentStore.h \
    headers/modelbase_api.h \
    src/modelbase.h
SOURCES += src/test_nodes/BinaryNodeAccessUnit.cpp \
    src/test_nodes/BinaryNode.cpp \
    src/test_nodes/BinaryWithPosition.cpp \
    test/ListTest.cpp \
    src/commands/ListRemove.cpp \
    src/commands/ListInsert.cpp \
    src/commands/NameChange.cpp \
    src/nodes/List.cpp \
    test/NotificationsTest.cpp \
    test/NotificationListener.cpp \
    src/nodes/Extendable/Attribute.cpp \
    src/nodes/Extendable/AttributeChain.cpp \
    src/nodes/Extendable/ExtendableIndex.cpp \
    src/nodes/Node.cpp \
    src/nodes/Extendable/ExtendableNode.cpp \
    src/commands/UndoCommand.cpp \
    test/ConcurrencyTests.cpp \
    src/commands/SetModificationTarget.cpp \
    src/NodeReadWriteLock.cpp \
    src/nodes/Reference.cpp \
    test/PersistenceTests.cpp \
    test/PersistentStoreMock.cpp \
    src/ModelException.cpp \
    test/PositionTests.cpp \
    src/nodes/Integer.cpp \
    test/UndoRedoTests.cpp \
    test/SimpleTests.cpp \
    src/nodes/Text.cpp \
    src/commands/ExtendedNodeOptional.cpp \
    src/Model.cpp \
    src/modelbase.cpp
