ENVISION_SRC_DIR = $$PWD/..
ENVISION_DIR = $${ENVISION_SRC_DIR}/envision_build
ENVISION_PLUGINS_DIR = $${ENVISION_DIR}/plugins
INCLUDEPATH += ./headers \
    ./src \
    ./test \
    $${ENVISION_SRC_DIR}/core/headers \
    $${ENVISION_SRC_DIR}
TARGET = modelbase
DEFINES += MODELBASE_LIBRARY
win32:LIBS += -L$${ENVISION_PLUGINS_DIR} \
    -llogger \
    -lselftest
QT = core \
    gui
QMAKE_CXXFLAGS += -Werror
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread
target.path = $$ENVISION_PLUGINS_DIR
pluginmeta.path = $$ENVISION_PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target \
    pluginmeta
debug:DEFINES += DEBUG
HEADERS += headers/commands/IntegerSet.h \
    headers/nodes/Integer.h \
    test/BinaryWithPosition.h \
    headers/commands/TextSet.h \
    headers/nodes/Text.h \
    headers/nodes/ExtendableNode.h \
    headers/nodeMacros.h \
    test/BinaryNode.h \
    headers/commands/ExtendedNodeOptional.h \
    headers/UndoCommand.h \
    headers/Model.h \
    headers/PersistentStore.h \
    headers/Node.h \
    headers/modelbase_api.h \
    src/modelbase.h
SOURCES += test/PositionTests.cpp \
    src/commands/IntegerSet.cpp \
    src/nodes/Integer.cpp \
    test/BinaryWithPosition.cpp \
    test/UndoRedoTests.cpp \
    test/SimpleTests.cpp \
    src/commands/TextSet.cpp \
    src/nodes/Text.cpp \
    test/BinaryNode.cpp \
    src/commands/ExtendedNodeOptional.cpp \
    src/UndoCommand.cpp \
    src/Model.cpp \
    src/Node.cpp \
    src/modelbase.cpp
