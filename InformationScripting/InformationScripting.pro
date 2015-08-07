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
    src/queries/AstNameFilter.h \
    src/queries/SubstractNodesOperator.h
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
    src/queries/AstNameFilter.cpp \
    src/queries/SubstractNodesOperator.cpp


# HACK to only include the AstApi_Generated file if it exists.
exists(src/wrappers/AstApi_Generated.cpp): {
    DEFINES+=AST_API_GENERATED
}

## The libboost_python is named differently on different systems. So find it with ldconfig.
# The below piece is borrowed from: https://github.com/mkeeter/antimony/blob/master/qt/python.pri
linux {
    # ldconfig is being used to find libboost_python, but it's in a different
    # place in different distros (and is not in the default $PATH on Debian).
    # First, check to see if it's on the default $PATH.
    system(which ldconfig > /dev/null) {
        LDCONFIG_BIN = "ldconfig"
    }
    # If that failed, then search for it in its usual places.
    isEmpty(LDCONFIG_BIN) {
        for(p, $$list(/sbin/ldconfig /usr/bin/ldconfig)) {
            exists($$p): LDCONFIG_BIN = $$p
        }
    }
    # If that search failed too, then exit with an error.
    isEmpty(LDCONFIG_BIN) {
        error("Could not find ldconfig!")
    }

    # Check for different boost::python naming schemes
    LDCONFIG_OUT = $$system($$LDCONFIG_BIN -p|grep python)
    for (b, $$list(boost_python-py34 boost_python3)) {
        contains(LDCONFIG_OUT, "lib$${b}.so") {
            LIBS += "-l$$b"
            GOT_BOOST_PYTHON = True
        }
    }

    # If we couldn't find boost::python, exit with an error.
    isEmpty(GOT_BOOST_PYTHON) {
        error("Could not find boost::python3")
    }
}
