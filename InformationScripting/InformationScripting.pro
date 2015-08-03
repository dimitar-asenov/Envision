TARGET = informationscripting
include(../Core/common_plugin.pri)

DEFINES += INFORMATIONSCRIPTING_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -loointeraction \

# UNIX PYTHON
unix:PYTHON_VERSION=3.4
unix:QMAKE_CXXFLAGS += $$system(python$${PYTHON_VERSION}-config --includes)
unix:LIBS += $$system(python$${PYTHON_VERSION}-config --libs)
# WIN PYTHON, NOTE UNTESTED: (adapted from: http://stackoverflow.com/a/1017194)
win32: PYTHON_VERSION=34
win32: INCLUDEPATH += Python$${PYTHON_VERSION}/include
win32: LIBS += -L/Python$${PYTHON_VERSION}/libs -lpython$${PYTHON_VERSION}


INCLUDEPATH += /usr/lib/boost
LIBS += -lboost_python3

HEADERS += src/precompiled.h \
    src/InformationScriptingException.h \
	src/informationscripting_api.h \
    src/InformationScriptingPlugin.h \
    src/commands/CScript.h \
    src/helpers/BoostPythonHelpers.h \
    src/wrappers/AstApi.h \
    src/graph/InformationNode.h \
    src/graph/Property.h \
    src/wrappers/NodeApi.h \
    src/graph/PropertyMap.h \
    src/graph/Graph.h \
    src/graph/InformationEdge.h \
    src/sources/AstSource.h \
    src/queries/Query.h \
    src/queries/AstQuery.h \
    src/queries/QueryExecutor.h \
    src/visualization/DefaultVisualizer.h \
    src/visitors/AllNodesOfType.h \
    src/queries/CompositeQuery.h \
    src/queries/GenericFilter.h \
    src/queries/AstNameFilter.h
SOURCES += src/InformationScriptingException.cpp \
	src/InformationScriptingPlugin.cpp \
	test/SimpleTest.cpp \
    src/commands/CScript.cpp \
    src/helpers/BoostPythonHelpers.cpp \
    src/wrappers/AstApi.cpp \
    src/graph/InformationNode.cpp \
    src/wrappers/NodeApi.cpp \
    src/graph/PropertyMap.cpp \
    src/graph/Property.cpp \
    src/graph/Graph.cpp \
    src/graph/InformationEdge.cpp \
    src/sources/AstSource.cpp \
    src/queries/AstQuery.cpp \
    src/queries/QueryExecutor.cpp \
    src/visualization/DefaultVisualizer.cpp \
    src/queries/CompositeQuery.cpp \
    src/queries/GenericFilter.cpp \
    src/queries/AstNameFilter.cpp


# HACK to only include the AstApi_Generated file if it exists.
exists(src/wrappers/AstApi_Generated.cpp): {
    DEFINES+=AST_API_GENERATED
}
