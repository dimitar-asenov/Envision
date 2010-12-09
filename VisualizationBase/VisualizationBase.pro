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
TARGET = visualizationbase
DEFINES += VISUALIZATIONBASE_LIBRARY
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
HEADERS += headers/layouts/SequentialLayoutStyle.h \
    headers/layouts/SequentialLayout.h \
    headers/ModelItem.h \
    headers/Item.h \
    headers/ModelRenderer.h \
    headers/ModelScene.h \
    headers/VisualizationManager.h \
    headers/View.h \
    headers/Scene.h \
    src/visualizationbase.h \
    headers/VisualizationException.h \
    headers/visualizationbase_api.h
SOURCES += src/layouts/SequentialLayoutStyle.cpp \
    src/layouts/SequentialLayout.cpp \
    src/ModelItem.cpp \
    src/Item.cpp \
    src/ModelRenderer.cpp \
    src/ModelScene.cpp \
    test/SimpleTest.cpp \
    src/VisualizationManager.cpp \
    src/visualizationbase.cpp \
    src/VisualizationException.cpp \
    src/View.cpp \
    src/Scene.cpp
