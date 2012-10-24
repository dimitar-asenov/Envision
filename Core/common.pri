ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -std=c++11 -pedantic-errors -Werror -Wextra
INCLUDEPATH += ./src ./test $${ENVISION_ROOT_DIR}
CONFIG += qt warn_on thread precompile_header
QT = core gui
PRECOMPILED_HEADER = src/precompiled.h