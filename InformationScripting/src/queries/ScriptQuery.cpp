/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "ScriptQuery.h"

#include "../python_bindings/BoostPythonHelpers.h"
#include "../query_framework/QueryRegistry.h"
#include "../query_prompt/parsing/QueryBuilder.h"
#include "../query_prompt/parsing/QueryParser.h"
#include "../query_prompt/nodes/QueryNode.h"

#include "ModelBase/src/nodes/Node.h"

#include "../query_framework/QueryRuntimeException.h"

namespace InformationScripting {

ScriptQuery::ScriptQuery(const QString& scriptPath, Model::Node* target,
								 const QStringList& args, QueryExecutor* executor)
	: LinearQuery{target}, scriptPath_{scriptPath}, arguments_{args}, executor_{executor}
{}

void ScriptQuery::unloadPythonEnvironment()
{
	Py_Finalize();
}

Optional<TupleSet> ScriptQuery::executeLinear(TupleSet input)
{
	using namespace boost;

	try {
		python::object main_module = python::import("__main__");
		python::dict main_namespace = python::extract<python::dict>(main_module.attr("__dict__"));

		python::object astApi = python::import("AstApi");
		python::object dataApi = python::import("DataApi");
		// Import modules to embedded environment:
		main_namespace["AstApi"] = astApi;
		importStar(main_namespace, astApi);
		main_namespace["DataApi"] = dataApi;
		importStar(main_namespace, dataApi);

		python::object sys = python::import("sys");

		python::object queryModule = python::import("Query");
		python::dict queriesDict = python::extract<python::dict>(queryModule.attr("__dict__"));
		main_namespace["Query"] = queryModule;

		// Provide Query interface:
		queriesDict["input"] = input;
		queriesDict["target"] = python::ptr(target());
		queriesDict["args"] = arguments_;
		queriesDict["result"] = TupleSet{};

		// Expose registered queries to the python environment:
		// Note when calling python scripts from python scripts:
		// 1: All data is shared between the scripts,
		// 	this could possibly be changed by calling Py_Initialize here in the execute method.
		// 2: It allows infinite recursion, e.g. in a script called test.py call test()
		auto allQueries = QueryRegistry::instance().registeredQueries()
				+ QueryRegistry::instance().scriptQueries()
				+ QueryRegistry::instance().aliasQueries();
		for (const auto& query : allQueries)
		{
			auto queryMethod = std::bind(&ScriptQuery::executeQueryFromPython, this, query, std::placeholders::_1,
												  std::placeholders::_2);
			auto call_policies = python::default_call_policies();
			using func_sig = boost::mpl::vector<QList<TupleSet>, python::list, python::list>;
			queriesDict[query] = python::make_function(queryMethod, call_policies, func_sig());
		}

		// export the build query function
		auto buildAndExecuteMethod = std::bind(&ScriptQuery::buildAndExecuteQueryFromPython, this, std::placeholders::_1,
															std::placeholders::_2);
		using func_sig = boost::mpl::vector<QList<TupleSet>, QString, python::list>;
		queriesDict["executeQuery"] = python::make_function(buildAndExecuteMethod,
																			 python::default_call_policies(), func_sig());

		// Per default exec_file does not set argv, thus we set it here manually.
		std::unique_ptr<wchar_t []> scriptPathArg{new wchar_t[scriptPath_.length() + 1]};
		int len = scriptPath_.toWCharArray(scriptPathArg.get());
		scriptPathArg[len] = '\0';
		wchar_t* argv[] = {scriptPathArg.get()};
		PySys_SetArgv(1, argv);

		exec_file(scriptPath_.toLatin1().data(), main_namespace, main_namespace);
		// Workaround to get output
		sys.attr("stdout").attr("flush")();

		return python::extract<TupleSet>(queriesDict["result"])();

	} catch (python::error_already_set& ) {
		return {QString{"Error in Python: %1"}.arg(BoostPythonHelpers::parsePythonException())};
	}
}

void ScriptQuery::importStar(boost::python::dict& main_namespace, boost::python::object apiObject)
{
	// equivalent to, e.g.: from AstApi import *:
	using namespace boost;

	python::dict astApiDict = python::extract<python::dict>(apiObject.attr("__dict__"));
	python::stl_input_iterator<python::object> keysBegin{astApiDict.keys()};
	python::stl_input_iterator<python::object> keysEnd;
	while (keysBegin != keysEnd)
	{
		QString key = python::extract<QString>{*keysBegin++};
		if (!key.startsWith("_")) main_namespace[key] = astApiDict[key];
	}
}

QList<TupleSet> ScriptQuery::executeQueryFromPython(QString name, boost::python::list args, boost::python::list input)
{
	boost::python::stl_input_iterator<QString> argsBegin{args};
	boost::python::stl_input_iterator<QString> argsEnd;
	QStringList argsConverted = QStringList::fromStdList(std::list<QString>{argsBegin, argsEnd});

	std::unique_ptr<Query> query{QueryRegistry::instance().buildQuery(name, target(), argsConverted, executor_)};
	return extractResult(query->execute(convertInput(input)), name);
}

QList<TupleSet> ScriptQuery::buildAndExecuteQueryFromPython(QString queryString, boost::python::list input)
{
	// TODO to properly use the parser from python we should probably remove assertions in it.
	std::unique_ptr<QueryNode> queryNode{QueryParser::parse(queryString)};
	QueryBuilder builder{target(), executor_};
	auto queries = builder.visit(queryNode.get());
	if (queries.size() > 1)
		throw QueryRuntimeException{"Yield is not allowed in queries built in python"};
	return extractResult(queries[0]->execute(convertInput(input)), "buildQuery");
}

QList<TupleSet> ScriptQuery::convertInput(boost::python::list input)
{
	boost::python::stl_input_iterator<TupleSet> inputsBegin{input};
	boost::python::stl_input_iterator<TupleSet> inputsEnd;
	return QList<TupleSet>::fromStdList(std::list<TupleSet>(inputsBegin, inputsEnd));
}

QList<TupleSet> ScriptQuery::extractResult(QList<Optional<TupleSet>> result, const QString& name)
{
	QList<TupleSet> noErrorResult;
	for (const auto& r : result)
	{
		if (r.hasErrors())
			throw QueryRuntimeException{QString{"Query %1 produced error results: %2"}.arg(name, r.errors().join(","))};
		else
			noErrorResult.push_back(r.value());
	}
	return noErrorResult;
}

}
