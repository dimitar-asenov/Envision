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
	if (args[0] == "script")
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
				auto infoNode = new InformationNode();
				infoNode->insert("ast", method);
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
	else if (args[0] == "methods")
	{
		auto query = AstSource::instance().createMethodQuery(node);
		QueryExecutor queryExecutor(query);
		queryExecutor.execute();
	}
	else if (args[0] == "bases")
	{
		auto query = AstSource::instance().createBaseClassesQuery(node);
		QueryExecutor queryExecutor(query);
		queryExecutor.execute();
	}
	return new Interaction::CommandResult();
}

} /* namespace InformationScripting */
