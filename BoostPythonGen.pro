TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle


SOURCES += main.cpp \
    GenTool.cpp \
    EnvisionAstConsumer.cpp \
    GeneratorAction.cpp \
    EnvisionPPCallbacks.cpp \
    TypeUtilities.cpp \
    APIData.cpp \
    APIPrinter.cpp

HEADERS += \
    GenTool.h \
    EnvisionAstConsumer.h \
    GeneratorAction.h \
    EnvisionPPCallbacks.h \
    TypeUtilities.h \
    APIData.h \
    APIPrinter.h

# Use this instead of the line under it to avoid warnings
QMAKE_CXXFLAGS += -std=c++1y -isystem ""$(shell $$_PRO_FILE_PWD_/llvm-config-envision.sh --includedir)"" -Wall -fno-rtti
#INCLUDEPATH +=  /usr/lib/llvm/include

DEFINES += __STDC_LIMIT_MACROS __STDC_CONSTANT_MACROS


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
                                ""$(shell $$_PRO_FILE_PWD_/llvm-config-envision.sh --libs)"" \
                                $$system( $$_PRO_FILE_PWD_/llvm-config-envision.sh --ldflags --libs cppbackend) \
                                -lz \
                                -ldl \
                                -lncurses
