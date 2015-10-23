TARGET = cppexport
include(../Core/common_plugin.pri)

DEFINES += CPPEXPORT_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel

HEADERS += src/precompiled.h \
    src/CppExportException.h \
	src/cppexport_api.h \
	src/CppExportPlugin.h \
	src/dependency_analysis/DependencyAnalyzer.h \
	src/dependency_analysis/DependencyUnit.h \
	src/dependency_analysis/DependencyTarget.h
SOURCES += src/CppExportException.cpp \
	src/CppExportPlugin.cpp \
	test/SimpleTest.cpp \
	src/dependency_analysis/DependencyAnalyzer.cpp \
	src/dependency_analysis/DependencyUnit.cpp
