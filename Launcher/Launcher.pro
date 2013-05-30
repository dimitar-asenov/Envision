include(../Core/common.pri)
TEMPLATE = app
TARGET = Envision
target.path = $$BUILD_DIR
INSTALLS += target
DEFINES += LAUNCHER
LIBS += -L$${BUILD_DIR} -lcore
HEADERS += src/precompiled.h
SOURCES += src/launcher.cpp
