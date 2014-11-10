TARGET = oodebug
include(../Core/common_plugin.pri)

DEFINES += OODEBUG_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -loointeraction \
    -lexport \
    -ljavaexport

HEADERS += src/precompiled.h \
    src/OODebugException.h \
	src/oodebug_api.h \
    src/OODebugPlugin.h \
    src/compiler/CompilerFeedback.h \
    src/compiler/CompilerOutputParser.h \
    src/compiler/CommandLineCompiler.h \
    src/compiler/CompilerMessage.h \
    src/compiler/java/JavaCompiler.h \
    src/commands/CJavaCompile.h \
    src/run_support/MainMethodFinder.h \
    src/run_support/java/JavaRunner.h \
    src/commands/CJavaRun.h \
    src/run_support/RunProcess.h \
    src/overlays/ConsoleOverlay.h \
    src/overlays/ConsoleOverlayStyle.h \
    src/handlers/HConsoleOverlay.h \
    src/debugger/jdwp/Protocol.h \
    src/debugger/jdwp/Message.h \
    src/debugger/jdwp/MessageItem.h \
    src/debugger/jdwp/messages/MessageHeader.h
SOURCES += src/OODebugException.cpp \
	src/OODebugPlugin.cpp \
	test/SimpleTest.cpp \
    src/compiler/CompilerOutputParser.cpp \
    src/compiler/CommandLineCompiler.cpp \
    src/compiler/java/JavaCompiler.cpp \
    src/commands/CJavaCompile.cpp \
    src/run_support/MainMethodFinder.cpp \
    src/run_support/java/JavaRunner.cpp \
    src/commands/CJavaRun.cpp \
    src/overlays/ConsoleOverlay.cpp \
    src/overlays/ConsoleOverlayStyle.cpp \
    src/run_support/RunProcess.cpp \
    src/handlers/HConsoleOverlay.cpp \
    src/debugger/jdwp/Protocol.cpp \
    src/debugger/jdwp/Message.cpp
