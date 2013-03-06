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



## LLVM SPECIFIC
LLVMCOMPONENTS = cppbackend

INCLUDEPATH += /usr/include/

DEFINES += _GNU_SOURCE __STDC_CONSTANT_MACROS __STDC_FORMAT_MACROS __STDC_LIMIT_MACROS


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
                                ""$(shell llvm-config --libs)"" \
                                $$system(llvm-config --ldflags --libs cppbackend)

## END LLVM SPECIFIC

HEADERS += src/precompiled.h \
    src/CppImportException.h \
	src/cppimport_api.h \
    src/cppimport.h \
    src/clangastvisitor.h \
    src/clangastconsumer.h \
    src/cppimportmanager.h \
    src/clangastfrontendactions.h \
    src/clangconsumerfactory.h \
    src/cppimportutilities.h \
    src/clangppfrontendaction.h \
    src/translatemanager.h
SOURCES += src/CppImportException.cpp \
	src/cppimport.cpp \
	test/SimpleTest.cpp \
    src/clangastvisitor.cpp \
    src/clangastconsumer.cpp \
    src/cppimportmanager.cpp \
    src/clangastfrontendactions.cpp \
    src/clangconsumerfactory.cpp \
    src/cppimportutilities.cpp \
    src/clangppfrontendaction.cpp \
    src/translatemanager.cpp
