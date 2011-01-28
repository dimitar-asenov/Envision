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
TARGET = oomodel
DEFINES += OOMODEL_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest \
    -lmodelbase
QT = core
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target \
    pluginmeta
HEADERS += headers/Statement.h \
    headers/MethodItem.h \
    headers/Method.h \
    headers/OOModelException.h \
    headers/common/Visibility.h \
    headers/common/attributeMacros.h \
    headers/Project.h \
    headers/Class.h \
    headers/Module.h \
    headers/oomodel_api.h \
    src/oomodel.h
SOURCES += src/Statement.cpp \
    src/MethodItem.cpp \
    src/Method.cpp \
    src/TypedListInstantiations.cpp \
    src/common/Visibility.cpp \
    src/OOModelException.cpp \
    src/Class.cpp \
    src/Project.cpp \
    src/Module.cpp \
    src/oomodel.cpp \
    test/SimpleTest.cpp
