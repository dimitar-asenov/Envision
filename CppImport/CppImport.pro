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


#QMAKE_CXXFLAGS -= -Werror


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
    src/ClangAstConsumer.h \
    src/ClangAstFrontendActions.h \
    src/ClangAstVisitor.h \
    src/ClangConsumerFactory.h \
    src/ClangPPFrontendAction.h \
    src/CppImportManager.h \
    src/CppImportUtilities.h \
    src/TranslateManager.h \
    src/ClangPPCallbacks.h \
    src/HelperSingleton.h
SOURCES += src/CppImportException.cpp \
	src/cppimport.cpp \
	test/SimpleTest.cpp \
    src/ClangAstConsumer.cpp \
    src/ClangAstFrontendActions.cpp \
    src/ClangAstVisitor.cpp \
    src/ClangConsumerFactory.cpp \
    src/ClangPPFrontendAction.cpp \
    src/CppImportManager.cpp \
    src/CppImportUtilities.cpp \
    src/TranslateManager.cpp \
    src/ClangPPCallbacks.cpp \
    src/HelperSingleton.cpp
