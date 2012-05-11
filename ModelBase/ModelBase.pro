TARGET = modelbase
include(../Core/common_plugin.pri)

DEFINES += MODELBASE_LIBRARY
win32:LIBS += -llogger \
    -lselftest

HEADERS += src/InterruptibleThread.h \
    src/Model.h \
    src/ModelException.h \
    src/NodeReadWriteLock.h \
    src/TypedListInstantiations.h \
    src/adapter/AdapterManager.h \
    src/commands/ExtendedNodeChild.h \
    src/commands/FieldSet.h \
    src/commands/ListInsert.h \
    src/commands/ListPut.h \
    src/commands/ListRemove.h \
    src/commands/NameChange.h \
    src/commands/PointerFieldSet.h \
    src/commands/UndoCommand.h \
    src/modelbase_api.h \
    src/nodes/Boolean.h \
    src/nodes/Character.h \
    src/nodes/Extendable/Attribute.h \
    src/nodes/Extendable/AttributeChain.h \
    src/nodes/Extendable/ExtendableIndex.h \
    src/nodes/Extendable/ExtendableNode.h \
    src/nodes/Float.h \
    src/nodes/Integer.h \
    src/nodes/List.h \
    src/nodes/Node.h \
    src/nodes/Reference.h \
    src/nodes/Text.h \
    src/nodes/TypedList.h \
    src/nodes/TypedListDefinition.h \
    src/nodes/nodeMacros.h \
    src/persistence/ClipboardStore.h \
    src/persistence/PersistedNode.h \
    src/persistence/PersistedValue.h \
    src/persistence/PersistentStore.h \
    src/precompiled.h \
    src/test_nodes/BinaryNode.h \
    src/test_nodes/BinaryNodeAccessUnit.h \
    src/test_nodes/BinaryNodePersistenceUnit.h \
    src/test_nodes/PartialList.h \
    src/test_nodes/PositionExtension.h \
    src/commands/AddModifiedNode.h \
    test/NotificationListener.h \
    src/commands/SetModificationTarget.h \
    test/PersistentStoreMock.h \
    src/modelbase.h
SOURCES += src/persistence/PersistentStore.cpp \
    src/adapter/AdapterManager.cpp \
    src/commands/AddModifiedNode.cpp \
    src/test_nodes/PositionExtension.cpp \
    src/TypedListInstantiations.cpp \
    src/nodes/Character.cpp \
    src/nodes/Boolean.cpp \
    src/nodes/Float.cpp \
    src/commands/ListPut.cpp \
    src/commands/ExtendedNodeChild.cpp \
    src/persistence/PersistedNode.cpp \
    src/test_nodes/BinaryNodePersistenceUnit.cpp \
    src/test_nodes/PartialList.cpp \
    src/test_nodes/BinaryNodeAccessUnit.cpp \
    src/test_nodes/BinaryNode.cpp \
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
    src/Model.cpp \
    src/modelbase.cpp
