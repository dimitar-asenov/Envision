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

# Use this instead of the line under it to avoid warnings
QMAKE_CXXFLAGS += -isystem ""$(shell $$_PRO_FILE_PWD_/llvm-config-envision.sh --includedir)""
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
				$$system( $$_PRO_FILE_PWD_/llvm-config-envision.sh --ldflags --libs cppbackend)

## END LLVM SPECIFIC

HEADERS += src/precompiled.h \
    src/CppImportException.h \
	src/cppimport_api.h \
    src/ClangAstConsumer.h \
    src/visitors/ClangAstVisitor.h \
    src/CppImportManager.h \
    src/CppImportUtilities.h \
    src/CppImportLogger.h \
    src/visitors/ExpressionVisitor.h \
    src/ClangFrontendActionFactory.h \
    src/visitors/TemplateArgumentVisitor.h \
    src/manager/NodeHasher.h \
    src/manager/TranslateManager.h \
    src/TranslateFrontendAction.h \
    src/CppImportPlugin.h
SOURCES += src/CppImportException.cpp \
	test/SimpleTest.cpp \
    src/ClangAstConsumer.cpp \
    src/visitors/ClangAstVisitor.cpp \
    src/CppImportManager.cpp \
    src/CppImportUtilities.cpp \
    src/CppImportLogger.cpp \
    src/visitors/ExpressionVisitor.cpp \
    src/ClangFrontendActionFactory.cpp \
    src/visitors/TemplateArgumentVisitor.cpp \
    src/manager/NodeHasher.cpp \
    src/manager/TranslateManager.cpp \
    src/TranslateFrontendAction.cpp \
    src/CppImportPlugin.cpp
