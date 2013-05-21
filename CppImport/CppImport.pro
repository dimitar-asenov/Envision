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
    src/visitors/ClangAstVisitor.h \
    src/ClangConsumerFactory.h \
    src/CppImportManager.h \
    src/CppImportUtilities.h \
    src/TranslateManager.h \
    src/ClangPPCallbacks.h \
    src/CppImportLogger.h \
    src/visitors/ExpressionVisitor.h
SOURCES += src/CppImportException.cpp \
	src/cppimport.cpp \
	test/SimpleTest.cpp \
    src/ClangAstConsumer.cpp \
    src/visitors/ClangAstVisitor.cpp \
    src/ClangConsumerFactory.cpp \
    src/CppImportManager.cpp \
    src/CppImportUtilities.cpp \
    src/TranslateManager.cpp \
    src/ClangPPCallbacks.cpp \
    src/CppImportLogger.cpp \
    src/visitors/ExpressionVisitor.cpp
