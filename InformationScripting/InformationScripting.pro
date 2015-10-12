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
    src/queries/Query.h \
    src/queries/AstQuery.h \
    src/queries/QueryExecutor.h \
    src/visualization/DefaultVisualizer.h \
    src/visitors/NodeGetter.h \
    src/queries/CompositeQuery.h \
    src/queries/GenericFilter.h \
    src/queries/AstNameFilter.h \
    src/queries/NodePropertyAdder.h \
    src/queries/UnionOperator.h \
    src/queries/ScriptQuery.h \
    src/dataformat/Tuple.h \
    src/dataformat/TupleSet.h \
    src/dataformat/Property.h \
    src/helpers/PythonSet.h \
    src/wrappers/DataApi.h \
    src/queries/QueryRegistry.h \
    src/queries/SubstractOperator.h \
    src/queries/AddASTPropertiesAsTuples.h \
    src/nodes/TagExtension.h \
    src/queries/TagQuery.h \
    src/queries/ScopedArgumentQuery.h \
    src/nodes/QueryNode.h \
    src/nodes/CommandNode.h \
    src/nodes/CommandArgument.h \
    src/nodes/CompositeQueryNode.h \
    src/visualization/QueryPrompt.h \
    src/nodes/QueryNodeContainer.h \
    src/visualization/VCommandNode.h \
    src/visualization/VCommandArgument.h \
    src/visualization/VCommandNodeStyle.h \
    src/visualization/VCommandArgumentStyle.h \
    src/handlers/HQuery.h \
    src/nodes/EmptyQueryNode.h \
    src/visualization/VEmptyQueryNode.h \
    src/visualization/VEmptyQueryNodeStyle.h \
    src/interaction/QueryOperatorDescriptor.h \
    src/interaction/QueryOperatorDescriptorList.h \
    src/nodes/UnfinishedQueryNode.h \
    src/visualization/VUnfinishedQueryNode.h \
    src/visualization/VUnfinishedQueryNodeStyle.h \
    src/parsing/QueryNodeBuilder.h \
    src/parsing/QueryParser.h \
    src/nodes/OperatorQueryNode.h \
    src/nodes/ErrorQueryNode.h \
    src/visualization/VErrorQueryNodeStyle.h \
    src/visualization/VErrorQueryNode.h \
    src/visualization/VOperatorQueryNode.h \
    src/visualization/VOperatorQueryNodeStyle.h \
    src/queries/LinearQuery.h \
    src/visualization/HighlightOverlay.h \
    src/visualization/Heatmap.h
SOURCES += src/InformationScriptingException.cpp \
	src/InformationScriptingPlugin.cpp \
	test/SimpleTest.cpp \
    src/commands/CScript.cpp \
    src/helpers/BoostPythonHelpers.cpp \
    src/wrappers/AstApi.cpp \
    src/queries/AstQuery.cpp \
    src/queries/QueryExecutor.cpp \
    src/visualization/DefaultVisualizer.cpp \
    src/queries/CompositeQuery.cpp \
    src/queries/GenericFilter.cpp \
    src/queries/AstNameFilter.cpp \
    src/queries/NodePropertyAdder.cpp \
    src/queries/UnionOperator.cpp \
    src/queries/ScriptQuery.cpp \
    src/dataformat/Tuple.cpp \
    src/dataformat/TupleSet.cpp \
    src/dataformat/Property.cpp \
    src/helpers/PythonSet.cpp \
    src/queries/QueryRegistry.cpp \
    src/queries/SubstractOperator.cpp \
    src/queries/AddASTPropertiesAsTuples.cpp \
    src/nodes/TagExtension.cpp \
    src/queries/TagQuery.cpp \
    src/queries/ScopedArgumentQuery.cpp \
    src/nodes/QueryNode.cpp \
    src/nodes/CommandNode.cpp \
    src/nodes/CommandArgument.cpp \
    src/nodes/CompositeQueryNode.cpp \
    src/visualization/QueryPrompt.cpp \
    src/nodes/QueryNodeContainer.cpp \
    src/visualization/VCommandNode.cpp \
    src/visualization/VCommandArgument.cpp \
    src/visualization/VCommandNodeStyle.cpp \
    src/visualization/VCommandArgumentStyle.cpp \
    src/handlers/HQuery.cpp \
    src/nodes/EmptyQueryNode.cpp \
    src/visualization/VEmptyQueryNode.cpp \
    src/visualization/VEmptyQueryNodeStyle.cpp \
    src/interaction/QueryOperatorDescriptor.cpp \
    src/interaction/QueryOperatorDescriptorList.cpp \
    src/nodes/UnfinishedQueryNode.cpp \
    src/visualization/VUnfinishedQueryNode.cpp \
    src/visualization/VUnfinishedQueryNodeStyle.cpp \
    src/parsing/QueryNodeBuilder.cpp \
    src/parsing/QueryParser.cpp \
    src/nodes/OperatorQueryNode.cpp \
    src/nodes/ErrorQueryNode.cpp \
    src/visualization/VErrorQueryNode.cpp \
    src/visualization/VErrorQueryNodeStyle.cpp \
    src/visualization/VOperatorQueryNode.cpp \
    src/visualization/VOperatorQueryNodeStyle.cpp \
    src/queries/LinearQuery.cpp \
    src/visualization/HighlightOverlay.cpp \
    src/visualization/Heatmap.cpp

# Workaround to not have any pragma's in NodeApi.cpp
# (because of unused local typedef in BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS):
# see also: http://stackoverflow.com/a/14608296
dataApiCompiler.input = SOURCES_DATAAPI
dataApiCompiler.output = ${QMAKE_FILE_BASE}.o
dataApiCompiler.commands = \$(CXX) -c \$(CXXFLAGS) \$(INCPATH) -Wno-unused-local-typedef ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
QMAKE_EXTRA_COMPILERS += dataApiCompiler

SOURCES_DATAAPI = src/wrappers/DataApi.cpp

# HACK to only include the AstApi_Generated file if it exists.
exists(src/wrappers/AstApi_Generated.cpp): {
    DEFINES+=AST_API_GENERATED
}

# Install script files into the script directory:
test_scripts.path = $${BUILD_DIR}/scripts
test_scripts.files = test/scripts/*
INSTALLS += test_scripts

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
