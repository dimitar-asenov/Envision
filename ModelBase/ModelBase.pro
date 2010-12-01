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
HEADERS += test/NotificationListener.h \
    headers/commands/UndoCommand.h \
    headers/nodes/nodeMacros.h \
    headers/nodes/Node.h \
    headers/nodes/Extendable/Attribute.h \
    headers/nodes/Extendable/AttributeChain.h \
    headers/nodes/Extendable/ExtendableIndex.h \
    headers/nodes/Extendable/ExtendableNode.h \
    test/BinaryNodeUnit.h \
    src/commands/SetModificationTarget.h \
    headers/InterruptibleThread.h \
    headers/NodeReadWriteLock.h \
    headers/commands/FieldSet.h \
    headers/nodes/Reference.h \
    test/PersistentStoreMock.h \
    headers/ModelException.h \
    headers/nodes/Integer.h \
    test/BinaryWithPosition.h \
    headers/nodes/Text.h \
    test/BinaryNode.h \
    headers/commands/ExtendedNodeOptional.h \
    headers/Model.h \
    headers/PersistentStore.h \
    headers/modelbase_api.h \
    src/modelbase.h
SOURCES += test/NotificationsTest.cpp \
    test/NotificationListener.cpp \
    src/nodes/Extendable/Attribute.cpp \
    src/nodes/Extendable/AttributeChain.cpp \
    src/nodes/Extendable/ExtendableIndex.cpp \
    src/nodes/Node.cpp \
    src/nodes/Extendable/ExtendableNode.cpp \
    src/commands/UndoCommand.cpp \
    test/ConcurrencyTests.cpp \
    test/BinaryNodeUnit.cpp \
    src/commands/SetModificationTarget.cpp \
    src/NodeReadWriteLock.cpp \
    src/nodes/Reference.cpp \
    test/PersistenceTests.cpp \
    test/PersistentStoreMock.cpp \
    src/ModelException.cpp \
    test/PositionTests.cpp \
    src/nodes/Integer.cpp \
    test/BinaryWithPosition.cpp \
    test/UndoRedoTests.cpp \
    test/SimpleTests.cpp \
    src/nodes/Text.cpp \
    test/BinaryNode.cpp \
    src/commands/ExtendedNodeOptional.cpp \
    src/Model.cpp \
    src/modelbase.cpp
