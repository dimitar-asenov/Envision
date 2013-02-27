TARGET = cppimport
include(../Core/common_plugin.pri)

DEFINES += CPPIMPORT_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -loointeraction


LIBS += -lclangTooling\
                                -lclangFrontendTool\
                                -lclangFrontend\
                                -lclangDriver\
                                -lclangSerialization\
                                -lclangCodeGen\
                                -lclangParse\
                                -lclangSema\
                                -lclangStaticAnalyzerFrontend\
                                -lclangStaticAnalyzerCheckers\
                                -lclangStaticAnalyzerCore\
                                -lclangAnalysis\
                                -lclangARCMigrate\
                                -lclangRewriteFrontend\
                                -lclangRewriteCore\
                                -lclangEdit\
                                -lclangAST\
                                -lclangLex\
                                -lclangBasic\
                                $(shell llvm-config --libs)

HEADERS += src/precompiled.h \
    src/CppImportException.h \
	src/cppimport_api.h \
    src/cppimport.h
SOURCES += src/CppImportException.cpp \
	src/cppimport.cpp \
	test/SimpleTest.cpp
