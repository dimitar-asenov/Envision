include(../Core/common.pri)
PRECOMPILED_HEADER =
TEMPLATE = app
TARGET = Envision
target.path = $$BUILD_DIR
INSTALLS += target
DEFINES += LAUNCHER
LIBS += -L$${BUILD_DIR} -lcore
HEADERS +=
SOURCES += src/launcher.cpp
