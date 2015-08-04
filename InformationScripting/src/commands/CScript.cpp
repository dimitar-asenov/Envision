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

#include "CScript.h"

#include "ModelBase/src/nodes/Node.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"

#include "../wrappers/AstApi.h"
#include "../wrappers/NodeApi.h"
#include "../helpers/BoostPythonHelpers.h"
#include "../graph/InformationNode.h"
#include "../queries/QueryExecutor.h"
#include "../queries/CompositeQuery.h"
#include "../queries/AstNameFilter.h"
#include "../queries/ComplementOperator.h"
#include "../sources/AstSource.h"

namespace InformationScripting {

CScript::CScript() : Command{"script"} {}

bool CScript::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens,
									const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() > 1 && commandTokens.first() == "script";
}

Interaction::CommandResult* CScript::execute(Visualization::Item*, Visualization::Item* target,
															const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	using namespace boost;

	auto node = target->node();
	Q_ASSERT(node);

	QStringList args = commandTokens.mid(1);
	if (!args.size()) new Interaction::CommandResult();

	QString command = args[0];
	args = args.mid(1);

	if (command == "script")
	{

		auto parentClass = node->firstAncestorOfType<OOModel::Class>();

		try {
			PyImport_AppendInittab("AstApi", PyInit_AstApi);
			PyImport_AppendInittab("NodeApi", PyInit_NodeApi);
			Py_Initialize();

			python::object main_module = python::import("__main__");
			python::dict main_namespace = python::extract<python::dict>(main_module.attr("__dict__"));

			python::object astApi = python::import("AstApi");
			python::object nodeApi = python::import("NodeApi");
			python::object sys = python::import("sys");

			python::list methods;
			for (auto method : *parentClass->methods())
			{
				// TODO: this leaks currently
				auto infoNode = new InformationNode({{"ast", method}});
				methods.append(python::ptr(infoNode));
			}

			main_namespace["methods"] = methods;

			exec_file("../InformationScripting/test/scripts/methods.py", main_namespace, main_namespace);
			// Workaround to get output
			sys.attr("stdout").attr("flush")();
		} catch (python::error_already_set ) {
			qDebug() << "Error in Python: " << BoostPythonHelpers::parsePythonException();
		}
	}
	else if (command == "methods")
	{
		auto query = AstSource::instance().createMethodQuery(node, args);
		auto compositeQuery = new CompositeQuery();
		compositeQuery->connectToOutput(query);
		QueryExecutor queryExecutor(compositeQuery);
		queryExecutor.execute();
	}
	else if (command == "bases")
	{
		auto query = AstSource::instance().createBaseClassesQuery(node, args);
		QueryExecutor queryExecutor(query);
		queryExecutor.execute();
	}
	else if (command == "pipe")
	{
		auto methodQuery = AstSource::instance().createMethodQuery(node, args);
		auto toBaseQuery = AstSource::instance().createToClassNodeQuery(node, args);
		auto compositeQuery = new CompositeQuery();
		compositeQuery->connectQuery(methodQuery, toBaseQuery);
		compositeQuery->connectToOutput(toBaseQuery);
		QueryExecutor queryExecutor(compositeQuery);
		queryExecutor.execute();
	}
	else if (command == "query21")
	{
		// Find all classes for which the name contains X and which have a method named Y
		// 5 queries seems like a lot for this :S
		auto classesQuery = AstSource::instance().createClassesQuery(node, {"g"});
		auto filterQuery = new AstNameFilter("Matcher");
		auto methodsOfQuery = AstSource::instance().createMethodQuery(node, {"of"});
		auto methodsFilter = new AstNameFilter("matches", true);
		auto toBaseQuery = AstSource::instance().createToClassNodeQuery(node, args);
		auto compositeQuery = new CompositeQuery();
		compositeQuery->connectQuery(classesQuery, filterQuery);
		compositeQuery->connectQuery(filterQuery, methodsOfQuery);
		compositeQuery->connectQuery(methodsOfQuery, methodsFilter);
		compositeQuery->connectQuery(methodsFilter, toBaseQuery);
		compositeQuery->connectToOutput(toBaseQuery);
		QueryExecutor queryExecutor(compositeQuery);
		queryExecutor.execute();
	}
	else if (command == "callgraph")
	{
		auto query = AstSource::instance().createCallgraphQuery(node, args);
		auto compositeQuery = new CompositeQuery();
		compositeQuery->connectToOutput(query);
		QueryExecutor queryExecutor(compositeQuery);
		queryExecutor.execute();
	}
	else if (command == "query19")
	{
		// Find all methods that are not called transitively from the TARGET method.
		auto allMethodsQuery = AstSource::instance().createMethodQuery(node, {"g"});
		auto callGraphQuery = AstSource::instance().createCallgraphQuery(node, args);
		auto complement = new ComplementOperator();
		auto compositeQuery = new CompositeQuery();
		compositeQuery->connectQuery(allMethodsQuery, complement);
		compositeQuery->connectQuery(callGraphQuery, 0, complement, 1);
		compositeQuery->connectToOutput(complement);
		QueryExecutor queryExecutor(compositeQuery);
		queryExecutor.execute();
	}
	return new Interaction::CommandResult();
}

} /* namespace InformationScripting */
