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
    -llogger
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
HEADERS += headers/commands/ExtendedNodeOptional.h \
    headers/UndoCommand.h \
    headers/Model.h \
    headers/PersistentStore.h \
    headers/ExtendableNode.h \
    headers/Node.h \
    headers/modelbase_api.h \
    src/modelbase.h
SOURCES += src/commands/ExtendedNodeOptional.cpp \
    src/UndoCommand.cpp \
    src/Model.cpp \
    src/Node.cpp \
    src/modelbase.cpp
