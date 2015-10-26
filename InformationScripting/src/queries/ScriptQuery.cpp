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

#include "../wrappers/AstApi.h"
#include "../wrappers/DataApi.h"
#include "../helpers/BoostPythonHelpers.h"
#include "../queries/QueryRegistry.h"

#include "ModelBase/src/nodes/Node.h"

namespace InformationScripting {

ScriptQuery::ScriptQuery(Query* parent, const QString& scriptPath, Model::Node* target, const QStringList& args)
	: Query{parent, target}, scriptPath_{scriptPath}, arguments_{args}
{}

// Since we can't create a module in another way, we create an empty one here.
// We fill it with the methods from the QueryRegistry during execution.
BOOST_PYTHON_MODULE(Query) {}

void ScriptQuery::initPythonEnvironment()
{
	PyImport_AppendInittab("AstApi", PyInit_AstApi);
	PyImport_AppendInittab("DataApi", PyInit_DataApi);
	PyImport_AppendInittab("Query", PyInit_Query);
	Py_Initialize();
}

void ScriptQuery::unloadPythonEnvironment()
{
	Py_Finalize();
}

QList<Optional<TupleSet> > ScriptQuery::execute(QList<TupleSet> input)
{
	using namespace boost;

	QList<Optional<TupleSet>> result;

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
		// Provide empty list to store results:
		main_namespace["results"] = python::list();

		python::object sys = python::import("sys");

		main_namespace["inputs"] = input;
		main_namespace["target"] = python::ptr(target());
		main_namespace["args"] = arguments_;

		python::object queryModule = python::import("Query");
		python::dict queriesDict = python::extract<python::dict>(queryModule.attr("__dict__"));
		main_namespace["Query"] = queryModule;

		// Expose registered queries to the python environment:
		// Note when calling python scripts from python scripts:
		// 1: All data is shared between the scripts,
		//		this could possibly be changed by calling Py_Initialize here in the execute method.
		// 2: It allows infinite recursion, e.g. in a script called test.py call test()
		auto allQueries = QueryRegistry::instance().registeredQueries() + QueryRegistry::instance().scriptQueries();
		for (const auto& query : allQueries)
		{
			auto queryMethod = std::bind(&ScriptQuery::queryExecutor, this, query, std::placeholders::_1,
												  std::placeholders::_2);
			auto call_policies = python::default_call_policies();
			using func_sig = boost::mpl::vector<QList<Optional<TupleSet>>, python::list, python::list>;
			queriesDict[query] = python::make_function(queryMethod, call_policies, func_sig());
		}

		exec_file(scriptPath_.toLatin1().data(), main_namespace, main_namespace);
		// Workaround to get output
		sys.attr("stdout").attr("flush")();

		python::list results = python::extract<python::list>(main_namespace["results"]);
		python::stl_input_iterator<TupleSet> begin(results), end;
		result = QList<Optional<TupleSet>>::fromStdList(std::list<Optional<TupleSet>>(begin, end));
	} catch (python::error_already_set ) {
		qDebug() << "Error in Python: " << BoostPythonHelpers::parsePythonException();
	}
	return result;
}

void ScriptQuery::importStar(boost::python::dict& main_namespace, boost::python::object apiObject)
{
	// equivalent to, e.g.: from AstApi import *:
	using namespace boost;

	python::dict astApiDict = python::extract<python::dict>(apiObject.attr("__dict__"));
			python::stl_input_iterator<python::object> keysBegin(astApiDict.keys()), keysEnd;
	while (keysBegin != keysEnd)
	{
		QString key = python::extract<QString>(*keysBegin++);
		if (!key.startsWith("_")) main_namespace[key] = astApiDict[key];
	}
}

QList<Optional<TupleSet>> ScriptQuery::queryExecutor(QString name, boost::python::list args, boost::python::list input)
{
	boost::python::stl_input_iterator<QString> argsBegin(args), argsEnd;
	QStringList argsConverted = QStringList::fromStdList(std::list<QString>(argsBegin, argsEnd));

	boost::python::stl_input_iterator<TupleSet> inputsBegin(input), inputsEnd;
	auto inputConverted = QList<TupleSet>::fromStdList(std::list<TupleSet>(inputsBegin, inputsEnd));

	std::unique_ptr<Query> query{QueryRegistry::instance().buildQuery(this, name, target(), argsConverted)};
	auto result = query->execute(inputConverted);

	return result;
}

} /* namespace InformationScripting */
