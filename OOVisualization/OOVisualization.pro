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
TARGET = oovisualization
DEFINES += OOVISUALIZATION_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase
QT = core \
    gui
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
styles.path = $${BUILD_DIR}/styles
styles.files = styles/*
INSTALLS += target \
    pluginmeta \
    styles
HEADERS += headers/icons/MethodIconStyle.h \
    headers/icons/MethodIcon.h \
    headers/icons/ClassIconStyle.h \
    headers/icons/ClassIcon.h \
    headers/vis/elements/VFieldStyle.h \
    headers/vis/elements/VField.h \
    headers/vis/top_level/VClassStyle.h \
    headers/vis/top_level/VClass.h \
    headers/vis/top_level/VModuleStyle.h \
    headers/vis/top_level/VModule.h \
    headers/vis/top_level/VProjectStyle.h \
    headers/vis/top_level/VProject.h \
    headers/vis/VStatic.h \
    headers/vis/VVisibility.h \
    headers/OOVisualizationException.h \
    headers/oovisualization_api.h \
    src/oovisualization.h
SOURCES += src/icons/MethodIcon.cpp \
    src/icons/MethodIconStyle.cpp \
    src/icons/ClassIcon.cpp \
    src/icons/ClassIconStyle.cpp \
    src/vis/elements/VFieldStyle.cpp \
    src/vis/elements/VField.cpp \
    src/vis/top_level/VMethodStyle.cpp \
    src/vis/top_level/VMethod.cpp \
    src/vis/top_level/VClass.cpp \
    src/vis/top_level/VClassStyle.cpp \
    src/vis/top_level/VModule.cpp \
    src/vis/top_level/VModuleStyle.cpp \
    src/vis/top_level/VProjectStyle.cpp \
    src/vis/top_level/VProject.cpp \
    src/vis/VStatic.cpp \
    src/vis/VVisibility.cpp \
    test/HelloWorldTest.cpp \
    src/OOVisualizationException.cpp \
    src/oovisualization.cpp
