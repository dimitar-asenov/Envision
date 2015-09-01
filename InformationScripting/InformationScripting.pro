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
    src/queries/SubstractNodesOperator.h \
    src/queries/NodePropertyAdder.h \
    src/queries/UnionOperator.h \
    src/queries/ScriptQuery.h \
    src/parsing/QueryBuilder.h
SOURCES += src/InformationScriptingException.cpp \
	src/InformationScriptingPlugin.cpp \
	test/SimpleTest.cpp \
    src/commands/CScript.cpp \
    src/helpers/BoostPythonHelpers.cpp \
    src/wrappers/AstApi.cpp \
    src/graph/InformationNode.cpp \
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
    src/queries/SubstractNodesOperator.cpp \
    src/queries/NodePropertyAdder.cpp \
    src/queries/UnionOperator.cpp \
    src/queries/ScriptQuery.cpp \
    src/visitors/AllNodesOfType.cpp \
    src/parsing/QueryBuilder.cpp

# Workaround to not have any pragma's in NodeApi.cpp
# (because of unused local typedef in BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS):
# see also: http://stackoverflow.com/a/14608296
nodeApiCompiler.input = SOURCES_NODEAPI
nodeApiCompiler.output = ${QMAKE_FILE_BASE}.o
nodeApiCompiler.commands = \$(CXX) -c \$(CXXFLAGS) \$(INCPATH) -Wno-unused-local-typedef ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
QMAKE_EXTRA_COMPILERS += nodeApiCompiler

SOURCES_NODEAPI = src/wrappers/NodeApi.cpp

# HACK to only include the AstApi_Generated file if it exists.
exists(src/wrappers/AstApi_Generated.cpp): {
    DEFINES+=AST_API_GENERATED
}

# The below piece is borrowed and adapted from: https://github.com/mkeeter/antimony/blob/master/qt/python.pri
cygwin {
    QMAKE_CXXFLAGS += $$system(python3-config --includes)
    LIBS += $$system(python3-config --libs)
    LIBS += -lboost_python3
}

macx {
    QMAKE_CXXFLAGS += $$system(/usr/local/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/local/bin/python3-config --ldflags)
    LIBS += -L/usr/local/lib -lboost_python3
    QMAKE_CXXFLAGS += -isystem/usr/local/include
}

linux {
    QMAKE_CXXFLAGS += $$system(python3-config --includes)
    LIBS += $$system(python3-config --libs)

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
