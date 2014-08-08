TARGET = export
include(../Core/common_plugin.pri)
DEFINES += EXPORT_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase
HEADERS += src/writer/FileWriter.h \
    src/writer/Exporter.h \
    src/writer/TextToNodeMap.h \
    src/writer/FragmentLayouter.h \
    src/tree/CompositeFragment.h \
    src/tree/TextFragment.h \
    src/tree/SourceFragment.h \
    src/tree/SourceFile.h \
    src/tree/SourceDir.h \
    src/precompiled.h \
    src/ExportException.h \
    src/export_api.h \
    src/ExportPlugin.h
SOURCES += src/writer/FileWriter.cpp \
    test/GenerationTests.cpp \
    src/writer/Exporter.cpp \
    src/writer/TextToNodeMap.cpp \
    src/writer/FragmentLayouter.cpp \
    src/tree/CompositeFragment.cpp \
    src/tree/TextFragment.cpp \
    src/tree/SourceFragment.cpp \
    src/tree/SourceFile.cpp \
    src/tree/SourceDir.cpp \
    src/ExportException.cpp \
    src/ExportPlugin.cpp
RESOURCES = Export.qrc
