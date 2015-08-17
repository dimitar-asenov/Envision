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
#include "../wrappers/NodeApi.h"
#include "../helpers/BoostPythonHelpers.h"

#include "../graph/Graph.h"

namespace InformationScripting {

ScriptQuery::ScriptQuery(const QString& scriptPath)
	: scriptPath_{scriptPath}
{}

QList<Graph*> ScriptQuery::execute(QList<Graph*> input)
{
	using namespace boost;

	QList<Graph*> result;

	try {
		PyImport_AppendInittab("AstApi", PyInit_AstApi);
		PyImport_AppendInittab("NodeApi", PyInit_NodeApi);
		Py_Initialize();

		python::object main_module = python::import("__main__");
		python::dict main_namespace = python::extract<python::dict>(main_module.attr("__dict__"));

		python::object astApi = python::import("AstApi");
		python::object nodeApi = python::import("NodeApi");
		python::object sys = python::import("sys");

		// TODO we have to consider how we can make sure that we don't have any memory leaks here:
		// In general we need to figure out memory management for the scripting environment, where ownership is clearly
		// defined.
		// (see also: http://stackoverflow.com/questions/28653886/delete-a-pointer-in-stdvector-exposed-by-boostpython)
		python::list inputGraphs;
		for (auto g : input)
			inputGraphs.append(python::ptr(g));

		main_namespace["inputs"] = inputGraphs;

		exec_file(scriptPath_.toLatin1().data(), main_namespace, main_namespace);
		// Workaround to get output
		sys.attr("stdout").attr("flush")();

		python::list results = python::extract<python::list>(main_namespace["results"]);
		python::stl_input_iterator<Graph*> begin(results), end;
		result = QList<Graph*>::fromStdList(std::list<Graph*>(begin, end));
	} catch (python::error_already_set ) {
		qDebug() << "Error in Python: " << BoostPythonHelpers::parsePythonException();
	}
	return result;
}

} /* namespace InformationScripting */
