ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -std=c++1y -pedantic-errors -Werror -Wextra -O2 -g -fno-omit-frame-pointer -Woverloaded-virtual

clang:QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''          # Used so that the main executable can link link to core
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/qt\''       # Used so that the main executable can link to custom built Qt
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/..\''       # Used so that plugins can link to core
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../qt\''    # Used so that plugins can link to custom built Qt

win32:QMAKE_LFLAGS += '-Wl,--export-all-symbols' #Export all symbols in Windows to save a hassle

unix:LIBS += -lprofiler

INCLUDEPATH += ./src ./test $${ENVISION_ROOT_DIR}
CONFIG += qt warn_on thread precompile_header debug_and_release depend_includepath no_keywords
QT = core gui widgets
PRECOMPILED_HEADER = src/precompiled.h
QMAKE_POST_LINK += $${ENVISION_ROOT_DIR}/checkers/vera++-check-dir $$_PRO_FILE_PWD_
