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

#include "QueryRegistry.h"

#include "../queries/AstQuery.h"
#include "../queries/AstNameFilter.h"
#include "../queries/NodePropertyAdder.h"
#include "../queries/ScriptQuery.h"

namespace InformationScripting {

QueryRegistry& QueryRegistry::instance()
{
	static QueryRegistry instance;
	return instance;
}

Query* QueryRegistry::buildQuery(const QString& command, Model::Node* target, QStringList args)
{
	if (auto constructor = constructors_[command])
		return constructor(target, args);
	if (args.size() > 1 && args[0] == "=")
	{
		// TODO we need some way to specify a condition on the node.
		// Or eventually decide that we don't allow condition in the property adder
		return new NodePropertyAdder(command, args[1]);
	}
	if (auto script = tryBuildQueryFromScript(command, args))
		return script;
	return nullptr;
}

QueryRegistry::QueryRegistry()
{
	registerDefaultQueries();
}

void QueryRegistry::registerDefaultQueries()
{
	registerQueryConstructor("classes", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::Classes, target, args);
	});
	registerQueryConstructor("methods", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::Methods, target, args);
	});
	registerQueryConstructor("bases", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::BaseClasses, target, args);
	});
	registerQueryConstructor("toClass", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::ToClass, target, args);
	});
	registerQueryConstructor("callgraph", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::CallGraph, target, args);
	});
	registerQueryConstructor("filter", [](Model::Node*, QStringList args) {
		Q_ASSERT(args.size());
		if (args[0].contains("*"))
		{
			QString regexString = args[0];
			regexString.replace("*", "\\w*");
			return new AstNameFilter(Model::SymbolMatcher(new QRegExp(regexString)));
		}
		return new AstNameFilter(Model::SymbolMatcher(args[0]));
	});
	registerQueryConstructor("script", [](Model::Node*, QStringList args) {
		QString scriptName = args.takeFirst();
		return new ScriptQuery(QString("../InformationScripting/test/scripts/%1.py").arg(scriptName), args);
	});
}

Query* QueryRegistry::tryBuildQueryFromScript(const QString& name, QStringList args)
{
	QString scriptName{scriptLocation_ + name + ".py"};
	if (QFile::exists(scriptName))
		return new ScriptQuery(scriptName, args);
	return nullptr;
}

} /* namespace InformationScripting */
