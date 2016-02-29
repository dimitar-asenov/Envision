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

#pragma once

#include "../informationscripting_api.h"

#include "LinearQuery.h"

namespace Model {
	class Node;
}

namespace InformationScripting {

class QueryExecutor;

class INFORMATIONSCRIPTING_API ScriptQuery : public LinearQuery
{
	public:
		ScriptQuery(const QString& scriptPath, Model::Node* target, const QStringList& args, QueryExecutor* executor);

		static void unloadPythonEnvironment();

		virtual Optional<TupleSet> executeLinear(TupleSet input) override;

	private:
		QString scriptPath_;
		// Note since we only register QList<T> to python we don't use QStringList here:
		QList<QString> arguments_;
		QueryExecutor* executor_{};

		void importStar(boost::python::dict& main_namespace, boost::python::object apiObject);

		QList<TupleSet> executeQueryFromPython(QString name, boost::python::list args, boost::python::list input);
		QList<TupleSet> buildAndExecuteQueryFromPython(QString queryString, boost::python::list input);

		/**
		 * Requires \a input to be a list of TupleSet.
		 */
		QList<TupleSet> convertInput(boost::python::list input);

		/**
		 * Extracts the values in \a result.
		 * In case a value in \a result has an error the function throws a \a QueryRuntimeException.
		 */
		QList<TupleSet> extractResult(QList<Optional<TupleSet>> result, const QString& name);
};

}
