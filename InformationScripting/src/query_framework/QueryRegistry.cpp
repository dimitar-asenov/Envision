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

#include "../queries/NodePropertyAdder.h"
#include "../queries/ScriptQuery.h"

#include "../query_prompt/parsing/QueryBuilder.h"
#include "../query_prompt/parsing/QueryParser.h"
#include "../query_prompt/nodes/QueryNode.h"

namespace InformationScripting {

QueryRegistry& QueryRegistry::instance()
{
	static QueryRegistry instance;
	return instance;
}

std::unique_ptr<Query> QueryRegistry::buildQuery(const QString& command, Model::Node* target,
																 QStringList args, QueryExecutor* executor)
{
	if (auto constructor = constructors_[command])
		return constructor(target, args, executor, {});
	if (args.size() > 1 && args[0] == "=")
	{
		// TODO we need some way to specify a condition on the node.
		// Or eventually decide that we don't allow condition in the property adder
		return std::unique_ptr<Query>(new NodePropertyAdder{command, args[1]});
	}
	auto result = tryBuildQueryFromAlias(command, target, args, executor);
	if (!result)
		result = tryBuildQueryFromScript(command, target, args, executor);
	return result;
}

QStringList QueryRegistry::scriptQueries() const
{
	QStringList result;
	for (const auto& scriptName : QDir{scriptLocation_}.entryList(QDir::Files | QDir::NoDotAndDotDot))
		if (scriptName.endsWith(".py"))
			result << scriptName.split(".py").takeFirst();
	return result;
}

QStringList QueryRegistry::aliasQueries() const
{
	QFile aliasFile("scripts/aliases");
	if (!aliasFile.open(QIODevice::ReadOnly))
	{
		// TODO report error...
		return {};
	}
	QTextStream aliasStream(&aliasFile);

	QStringList result;
	while (!aliasStream.atEnd())
	{
		QString aliasLine = aliasStream.readLine();
		if (aliasLine.isEmpty() || aliasLine.startsWith("#"))
			continue;

		QStringList parts = aliasLine.split("=");
		if (parts.size() > 1)
			result.append( parts.first().trimmed() );
	}

	return result;
}

std::unique_ptr<Query> QueryRegistry::tryBuildQueryFromScript(const QString& name, Model::Node* target,
																				  QStringList args, QueryExecutor* executor)
{
	QString scriptName{scriptLocation_ + name + ".py"};
	if (QFile::exists(scriptName))
		return std::unique_ptr<Query>(new ScriptQuery{scriptName, target, args, executor});
	return nullptr;
}

std::unique_ptr<Query> QueryRegistry::tryBuildQueryFromAlias(const QString& name, Model::Node* target,
																				 QStringList args, QueryExecutor* executor)
{
	QFile aliasFile("scripts/aliases");
	if (!aliasFile.open(QIODevice::ReadOnly))
	{
		// TODO report error...
		return {};
	}
	QTextStream aliasStream(&aliasFile);

	std::unique_ptr<Query> result{};

	while (!result && !aliasStream.atEnd())
	{
		QString aliasLine = aliasStream.readLine();
		if (aliasLine.isEmpty() || aliasLine.startsWith("#"))
			continue;

		QStringList parts = aliasLine.split("=");
		if (parts.size() <= 1)
		{
			qWarning() << "Ignoring alias line because no = found:" << aliasLine;
			continue;
		}
		QString aliasName = parts.takeFirst().trimmed();
		if (name == aliasName)
		{
			QString aliasedQuery = parts.join('=').trimmed();

			// TODO we can be fancy with the single dollar
			if (aliasedQuery.contains("$$"))
				aliasedQuery.replace("$$", args.join(' '));
			else
				aliasedQuery.append(' ').append(args.join(' '));

			// TODO to properly use the parser from python we should probably remove assertions in it.
			std::unique_ptr<QueryNode> queryNode{QueryParser::parse(aliasedQuery)};
			QueryBuilder builder{target, executor};
			auto queries = builder.visit(queryNode.get());
			if (queries.size() == 1)
				result = std::move(queries[0]);
			// TODO handle else case properly
		}
	}

	return result;
}

}
