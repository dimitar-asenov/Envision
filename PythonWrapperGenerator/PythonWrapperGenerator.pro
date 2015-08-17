TEMPLATE = app
CONFIG += console precompile_header
CONFIG -= app_bundle

ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild

#Workaround: https://bugreports.qt.io/browse/QTBUG-27018
CONFIG -= clang_pch_style

SOURCES += src/main.cpp \
    src/GenTool.cpp \
    src/EnvisionAstConsumer.cpp \
    src/GeneratorAction.cpp \
    src/EnvisionPPCallbacks.cpp \
    src/TypeUtilities.cpp \
    src/APIData.cpp \
    src/APIPrinter.cpp \
    src/Config.cpp

HEADERS += \
    src/GenTool.h \
    src/EnvisionAstConsumer.h \
    src/GeneratorAction.h \
    src/EnvisionPPCallbacks.h \
    src/TypeUtilities.h \
    src/APIData.h \
    src/APIPrinter.h \
    src/Config.h \
    src/precompiled.h

PRECOMPILED_HEADER = src/precompiled.h

# Use this instead of the line under it to avoid warnings
QMAKE_CXXFLAGS += -std=c++1y -isystem ""$(shell $${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --includedir)"" \
                  -Wall -fno-rtti

DEFINES += __STDC_LIMIT_MACROS __STDC_CONSTANT_MACROS _GLIBCXX_USE_CXX11_ABI=0


LIBS += -lclangTooling\
                                -lclangFrontendTool\
                                -lclangFrontend\
                                -lclangDriver\
                                -lclangSerialization\
                                -lclangParse\
                                -lclangSema\
                                -lclangAnalysis \
                                -lclangEdit\
                                -lclangAST\
                                -lclangLex\
                                -lclangBasic\
                                $$system( $${ENVISION_ROOT_DIR}/misc/llvm-config-envision.sh --ldflags \
                                                            --libs cppbackend analysis codegen bitreader option core) \
                                -lz \
                                -ldl \
                                -lncurses

## INSTALLING SPECIFICS:
config.files=config.json
config.path=$${BUILD_DIR}/tools

target.path = $${BUILD_DIR}/tools
INSTALLS += target config
