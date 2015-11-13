TARGET = cppexport
include(../Core/common_plugin.pri)

DEFINES += CppExport_EXPORTS
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
	src/dependency_analysis/DependencyTarget.h \
	src/Config.h \
	src/dependency_analysis/DependencyComposite.h
SOURCES += src/CppExportException.cpp \
	src/CppExportPlugin.cpp \
	test/SimpleTest.cpp \
	src/dependency_analysis/DependencyAnalyzer.cpp \
	src/dependency_analysis/DependencyUnit.cpp \
	src/Config.cpp \
	src/dependency_analysis/DependencyComposite.cpp

## INSTALLING SPECIFICS:
config.files=config.json
config.path=$${BUILD_DIR}/cpp-export-settings

INSTALLS += config
