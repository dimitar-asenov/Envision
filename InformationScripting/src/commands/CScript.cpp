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
#include "ModelBase/src/util/SymbolMatcher.h"

#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"

#include "../queries/QueryExecutor.h"
#include "../queries/CompositeQuery.h"
#include "../queries/SubstractOperator.h"
#include "../queries/NodePropertyAdder.h"
#include "../queries/UnionOperator.h"
#include "../queries/AddASTPropertiesAsTuples.h"
#include "../queries/QueryRegistry.h"
#include "../parsing/QueryParser.h"
#include "../parsing/QueryParsingException.h"

#include "../visualization/QueryPrompt.h"


namespace InformationScripting {

CScript::CScript() : Command{"script"} {}

bool CScript::canInterpret(Visualization::Item* source, Visualization::Item*, const QStringList& commandTokens,
									const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() > 1 && commandTokens.first() == "script" && source->findAncestorWithNode();
}

Interaction::CommandResult* CScript::execute(Visualization::Item* source, Visualization::Item*,
															const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	using namespace boost;

	auto node = source->findAncestorWithNode()->node();
	Q_ASSERT(node);

	QStringList args = commandTokens.mid(1);
	if (!args.size()) new Interaction::CommandResult();

	QString command = args[0];
	args = args.mid(1);

	if (command == "x")
	{
		new QueryPrompt(source);
	}
	else if (command != "fallback")
	{
		args.prepend(command);
		Query* q = nullptr;
		try {
			q = QueryParser::buildQueryFrom(args.join(""), node);
		} catch (const QueryParsingException& e) {
			return new Interaction::CommandResult(new Interaction::CommandError(e.message()));
		}
		Q_ASSERT(q);
		QueryExecutor queryExecutor(q);
		return queryExecutor.execute();
	}
	else
	{
		auto query = [](auto... args){return QueryRegistry::instance().buildQuery(args...);};

		QString command = args[0];
		args = args.mid(1);

		if (command == "script")
		{
			if (args.size() > 1)
			{
				// $<"<classes g>" | "<script test>">$
				auto classesQuery = query("classes", node, QStringList("-s=g"));
				// TODO we could be more fancy in script file name detection, e.g. if .py is already entered don't append it.
				auto scriptQuery = query(args.takeFirst(), node, args);
				auto compositeQuery = new CompositeQuery();
				compositeQuery->connectQuery(classesQuery, scriptQuery);
				compositeQuery->connectToOutput(scriptQuery);
				QueryExecutor queryExecutor(compositeQuery);
				queryExecutor.execute();
			}
		}
		else if (command == "methods")
		{
			// "<methods>"
			auto methodsQuery = query("methods", node, args);
			QueryExecutor queryExecutor(methodsQuery);
			queryExecutor.execute();
		}
		else if (command == "bases")
		{
			// "<bases>"
			auto basesQuery = query("bases", node, args);
			QueryExecutor queryExecutor(basesQuery);
			queryExecutor.execute();
		}
		else if (command == "pipe")
		{
			// $<"<methods>" | "<toClass>">$
			auto methodQuery = query("methods", node, args);
			auto toBaseQuery = query("toClass", node, args);
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

			// $<"<classes -s=g>" | "<filter *Matcher*>" | "<methods -s=of>" | "<filter matches>" | "<toClass>">$
			auto classesQuery = query("classes", node, QStringList("-s=g"));
			auto filterQuery = query("filter", node, QStringList("*Matcher*"));
			auto methodsOfQuery = query("methods", node, QStringList("-s=of"));
			auto methodsFilter = query("filter", node, QStringList("matches"));
			auto toBaseQuery = query("toClass", node, args);
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
			// "<callgraph>"
			auto callgraph = query("callgraph", node, args);
			auto compositeQuery = new CompositeQuery();
			compositeQuery->connectToOutput(callgraph);
			QueryExecutor queryExecutor(compositeQuery);
			queryExecutor.execute();
		}
		else if (command == "query19")
		{
			// Find all methods that are not called transitively from the TARGET method.

			// $<"<methods -s=g>" - {<$<"<callgraph>" | "<addASTProperties>">$>}>$
			// or:
			// $<{<"<methods -s=g>", $<"<callgraph>" | "<addASTProperties>">$>}->$
			auto allMethodsQuery = query("methods", node, QStringList("-s=g"));
			auto callGraphQuery = query("callgraph", node, args);
			auto astAdder = new AddASTPropertiesAsTuples();
			auto complement = new SubstractOperator();
			auto compositeQuery = new CompositeQuery();
			compositeQuery->connectQuery(callGraphQuery, astAdder);
			compositeQuery->connectQuery(allMethodsQuery, complement);
			compositeQuery->connectQuery(astAdder, 0, complement, 1);
			compositeQuery->connectToOutput(complement);
			QueryExecutor queryExecutor(compositeQuery);
			queryExecutor.execute();
		}
		else if (command == "color")
		{
			// $<"<methods>" | {<$<"<filter *quotes*>" | "<color = blue>">$, $<"<filter *brackets*>" | "<color = green>">$>} U>$
			auto colorQuotes = new NodePropertyAdder("color", QString("blue"));
			auto colorBrackets = new NodePropertyAdder("color", QString("green"));

			auto quotesFilter = query("filter", node, QStringList("*quotes*"));
			auto bracketsFilter = query("filter", node, QStringList("*brackets*"));

			auto quotes = new CompositeQuery();
			quotes->connectInput(0, quotesFilter);
			quotes->connectQuery(quotesFilter, colorQuotes);
			quotes->connectToOutput(colorQuotes);

			auto brackets = new CompositeQuery();
			brackets->connectInput(0, bracketsFilter);
			brackets->connectQuery(bracketsFilter, colorBrackets);
			brackets->connectToOutput(colorBrackets);

			auto methods = query("methods", node, QStringList());
			auto unionOp = new UnionOperator();

			auto composite = new CompositeQuery();

			composite->connectQuery(methods, quotes);
			composite->connectQuery(methods, brackets);

			composite->connectQuery(quotes, unionOp);
			composite->connectQuery(brackets, 0, unionOp, 1);

			composite->connectToOutput(unionOp);

			QueryExecutor queryExecutor(composite);
			queryExecutor.execute();
		}
	}
	return new Interaction::CommandResult();
}

} /* namespace InformationScripting */
