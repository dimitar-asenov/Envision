TARGET = informationscripting
include(../Core/common_plugin.pri)

DEFINES += InformationScripting_EXPORTS
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase \
    -loovisualization \
    -loointeraction \
    -loodebug \

INCLUDEPATH += /usr/lib/boost

HEADERS += src/precompiled.h \
    src/InformationScriptingException.h \
    src/informationscripting_api.h \
    src/InformationScriptingPlugin.h \
    src/queries/Query.h \
    src/queries/AstQuery.h \
    src/queries/CompositeQuery.h \
    src/queries/NodePropertyAdder.h \
    src/queries/UnionOperator.h \
    src/queries/ScriptQuery.h \
    src/dataformat/Tuple.h \
    src/dataformat/TupleSet.h \
    src/dataformat/Property.h \
    src/queries/SubstractOperator.h \
    src/queries/AddASTPropertiesAsTuples.h \
    src/nodes/TagExtension.h \
    src/queries/TagQuery.h \
    src/queries/LinearQuery.h \
    src/queries/VersionControlQuery.h \
    src/queries/BreakpointManager.h \
    src/queries/RuntimeQuery.h \
    src/query_prompt/QueryPromptMode.h \
    src/queries/PassthroughQuery.h \
    src/query_prompt/nodes/CommandArgument.h \
    src/query_prompt/nodes/CommandNode.h \
    src/query_prompt/nodes/CompositeQueryNode.h \
    src/query_prompt/nodes/OperatorQueryNode.h \
    src/query_prompt/nodes/QueryNode.h \
    src/query_prompt/nodes/QueryNodeContainer.h \
    src/query_prompt/visualization/VOperatorQueryNodeStyle.h \
    src/query_prompt/visualization/VOperatorQueryNode.h \
    src/query_prompt/visualization/VCompositeQueryNodeStyle.h \
    src/query_prompt/visualization/VCompositeQueryNode.h \
    src/query_prompt/visualization/VCommandNode.h \
    src/query_prompt/visualization/VCommandNodeStyle.h \
    src/query_prompt/visualization/VCommandArgumentStyle.h \
    src/query_prompt/visualization/VCommandArgument.h \
    src/query_prompt/parsing/QueryBuilder.h \
    src/query_prompt/visualization/VQueryNodeContainer.h \
    src/query_prompt/visualization/QueryPromptInputStyle.h \
    src/query_prompt/visualization/QueryPromptInput.h \
    src/query_prompt/HQuery.h \
    src/python_bindings/AstApi.h \
    src/python_bindings/DataApi.h \
    src/python_bindings/BoostPythonHelpers.h \
    src/python_bindings/PythonSet.h \
    src/python_bindings/AstModification.h \
    src/query_framework/Optional.h \
    src/query_framework/QueryRegistry.h \
    src/query_framework/QueryExecutor.h \
    src/query_framework/ArgumentParser.h \
    src/query_framework/ArgumentRule.h \
    src/query_framework/DefaultVisualizer.h \
    src/query_framework/HighlightOverlay.h \
    src/queries/Heatmap.h \
    src/query_prompt/parsing/QueryParser.h \
    src/query_framework/QueryParsingException.h \
    src/queries/Count.h \
    src/queries/Join.h
SOURCES += src/InformationScriptingException.cpp \
    src/InformationScriptingPlugin.cpp \
    test/SimpleTest.cpp \
    src/queries/AstQuery.cpp \
    src/queries/CompositeQuery.cpp \
    src/queries/NodePropertyAdder.cpp \
    src/queries/UnionOperator.cpp \
    src/queries/ScriptQuery.cpp \
    src/dataformat/Tuple.cpp \
    src/dataformat/TupleSet.cpp \
    src/dataformat/Property.cpp \
    src/queries/SubstractOperator.cpp \
    src/queries/AddASTPropertiesAsTuples.cpp \
    src/nodes/TagExtension.cpp \
    src/queries/TagQuery.cpp \
    src/queries/LinearQuery.cpp \
    src/queries/VersionControlQuery.cpp \
    src/queries/BreakpointManager.cpp \
    src/queries/RuntimeQuery.cpp \
    src/query_prompt/QueryPromptMode.cpp \
    src/queries/PassthroughQuery.cpp \
    src/query_prompt/nodes/CommandArgument.cpp \
    src/query_prompt/nodes/CommandNode.cpp \
    src/query_prompt/nodes/CompositeQueryNode.cpp \
    src/query_prompt/nodes/OperatorQueryNode.cpp \
    src/query_prompt/nodes/QueryNode.cpp \
    src/query_prompt/nodes/QueryNodeContainer.cpp \
    src/query_prompt/visualization/QueryPromptInput.cpp \
    src/query_prompt/visualization/QueryPromptInputStyle.cpp \
    src/query_prompt/parsing/QueryBuilder.cpp \
    src/query_prompt/visualization/VQueryNodeContainer.cpp \
    src/query_prompt/visualization/VOperatorQueryNodeStyle.cpp \
    src/query_prompt/visualization/VOperatorQueryNode.cpp \
    src/query_prompt/visualization/VCompositeQueryNodeStyle.cpp \
    src/query_prompt/visualization/VCompositeQueryNode.cpp \
    src/query_prompt/visualization/VCommandNodeStyle.cpp \
    src/query_prompt/visualization/VCommandArgumentStyle.cpp \
    src/query_prompt/visualization/VCommandArgument.cpp \
    src/query_prompt/visualization/VCommandNode.cpp \
    src/query_prompt/HQuery.cpp \
    src/python_bindings/AstApi.cpp \
    src/python_bindings/DataApi.cpp \
    src/python_bindings/BoostPythonHelpers.cpp \
    src/python_bindings/PythonSet.cpp \
    src/python_bindings/AstModification.cpp \
    src/query_framework/QueryExecutor.cpp \
    src/query_framework/QueryRegistry.cpp \
    src/query_framework/ArgumentParser.cpp \
    src/query_framework/ArgumentRule.cpp \
    src/query_framework/DefaultVisualizer.cpp \
    src/queries/Heatmap.cpp \
    src/query_framework/HighlightOverlay.cpp \
    src/query_prompt/parsing/QueryParser.cpp \
    src/query_framework/QueryParsingException.cpp \
    src/queries/Count.cpp \
    src/queries/Join.cpp

# The workaround below is currently not needed but might be in the future, thus we leave it as reference.
# Workaround to not have any pragma's in NodeApi.cpp
# (because of unused local typedef in BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS):
# see also: http://stackoverflow.com/a/14608296
#dataApiCompiler.input = SOURCES_DATAAPI
#dataApiCompiler.output = ${QMAKE_FILE_BASE}.o
#dataApiCompiler.commands = \$(CXX) -c \$(CXXFLAGS) \$(INCPATH) -Wno-unused-local-typedef ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
#QMAKE_EXTRA_COMPILERS += dataApiCompiler

#SOURCES_DATAAPI = src/python_bindings/DataApi.cpp

# HACK to only include the AstApi_Generated file if it exists.
exists(src/python_bindings/AstApi_Generated.cpp): {
    DEFINES+=AST_API_GENERATED
}

# Install script files into the script directory:
scripts.path = $${BUILD_DIR}/scripts
scripts.files = scripts/*
INSTALLS += scripts

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
