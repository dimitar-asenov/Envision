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
    src/commands/CJavaCompile.h \
    src/run_support/MainMethodFinder.h \
    src/commands/CJavaRun.h \
    src/run_support/RunProcess.h \
    src/overlays/ConsoleOverlay.h \
    src/overlays/ConsoleOverlayStyle.h \
    src/handlers/HConsoleOverlay.h \
    src/debugger/jdwp/Protocol.h \
    src/debugger/jdwp/MessageField.h \
    src/debugger/jdwp/DebugConnector.h \
    src/debugger/jdwp/Reply.h \
    src/debugger/jdwp/Command.h \
    src/commands/CJavaDebug.h \
    src/debugger/JavaDebugger.h \
    src/debugger/jdwp/MessagePart.h \
    src/debugger/jdwp/Location.h \
    src/debugger/jdwp/messages/EventSet.h \
    src/debugger/jdwp/messages/EventRequestSet.h \
    src/debugger/jdwp/messages/VMSet.h \
    src/debugger/jdwp/messages/AllMessages.h \
    src/debugger/jdwp/messages/ReferenceTypeSet.h \
    src/debugger/jdwp/messages/MethodSet.h \
    src/debugger/jdwp/messages/ThreadSet.h \
    src/debugger/jdwp/messages/StackFrameSet.h \
    src/debugger/jdwp/messages/StringSet.h \
    src/debugger/ReferenceFinder.h \
    src/compiler/JavaCompiler.h \
    src/run_support/JavaRunner.h
SOURCES += src/OODebugException.cpp \
	src/OODebugPlugin.cpp \
	test/SimpleTest.cpp \
    src/compiler/CompilerOutputParser.cpp \
    src/compiler/CommandLineCompiler.cpp \
    src/commands/CJavaCompile.cpp \
    src/run_support/MainMethodFinder.cpp \
    src/commands/CJavaRun.cpp \
    src/overlays/ConsoleOverlay.cpp \
    src/overlays/ConsoleOverlayStyle.cpp \
    src/run_support/RunProcess.cpp \
    src/handlers/HConsoleOverlay.cpp \
    src/debugger/jdwp/Protocol.cpp \
    src/debugger/jdwp/DebugConnector.cpp \
    src/commands/CJavaDebug.cpp \
    src/debugger/JavaDebugger.cpp \
    src/debugger/jdwp/MessagePart.cpp \
    src/debugger/jdwp/Reply.cpp \
    src/debugger/jdwp/Command.cpp \
    src/debugger/jdwp/Location.cpp \
    src/debugger/jdwp/messages/EventSet.cpp \
    src/debugger/jdwp/messages/EventRequestSet.cpp \
    src/debugger/jdwp/messages/VMSet.cpp \
    src/debugger/jdwp/messages/ReferenceTypeSet.cpp \
    src/debugger/jdwp/messages/MethodSet.cpp \
    src/debugger/jdwp/messages/ThreadSet.cpp \
    src/debugger/jdwp/messages/StackFrameSet.cpp \
    src/debugger/jdwp/messages/StringSet.cpp \
    src/debugger/ReferenceFinder.cpp \
    src/compiler/JavaCompiler.cpp \
    src/run_support/JavaRunner.cpp
