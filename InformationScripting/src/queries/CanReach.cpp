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

#include "CanReach.h"

#include "../query_framework/QueryRegistry.h"

#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/util/SymbolMatcher.h"

namespace InformationScripting {

const QStringList CanReach::NAME_ARGUMENT_NAMES{"n", "name"};
const QStringList CanReach::RELATION_ARGUMENT_NAMES{"r", "relation"};
const QStringList CanReach::SELF_ARGUMENT_NAMES{"s", "self"};

Optional<TupleSet> CanReach::executeLinear(TupleSet input)
{
	QString relationName = arguments_.argument(RELATION_ARGUMENT_NAMES[1]);
	QString targetName = arguments_.argument(NAME_ARGUMENT_NAMES[1]);
	auto nameMatcher = Model::SymbolMatcher::guessMatcher(targetName);

	std::vector<Tuple> endNodes;

	relationTuples_ = input.tuples(relationName);
	// First we search all end points, i.e. relations where the second entry matches the target name.
	// Then from the endpoints we can do BFS and mark all found nodes as nodes that can reach the target.
	for (const auto& relation : relationTuples_)
	{
		auto nodes = relation.valuesOfType<Model::Node*>();
		if (nodes.size() != 2)
			return {QString{"%1 works only on relations between Nodes."}.arg(arguments_.queryName())};
		if (matchSelf_ || nameMatcher.matches(nodes[1]->symbolName()))
			endNodes.push_back(relation);
	}
	return reachableNodesFrom(std::move(endNodes));
}

void CanReach::registerDefaultQueries()
{
	QueryRegistry::registerQuery<CanReach>("canReach",
		std::vector<ArgumentRule>{{ArgumentRule::RequireAll, {{RELATION_ARGUMENT_NAMES[1]}}},
											{ArgumentRule::RequireOneOf, {{NAME_ARGUMENT_NAMES[1]},
																					{SELF_ARGUMENT_NAMES[1], ArgumentValue::IsSet}}}});
}

CanReach::CanReach(Model::Node* target, QStringList args, std::vector<ArgumentRule> argumentRules)
	: LinearQuery{target}, arguments_{{
		{NAME_ARGUMENT_NAMES, "Name of the target to reach", NAME_ARGUMENT_NAMES[1]},
		{RELATION_ARGUMENT_NAMES, "Name of the relation to follow", RELATION_ARGUMENT_NAMES[1]},
		QCommandLineOption{SELF_ARGUMENT_NAMES}
	}, args}
{
	for (const auto& rule : argumentRules)
		rule.check(arguments_);
	matchSelf_ = arguments_.isArgumentSet(SELF_ARGUMENT_NAMES[1]);
}

TupleSet CanReach::reachableNodesFrom(std::vector<Tuple> startNodes)
{
	TupleSet result;
	for (const auto& start : startNodes)
	{
		std::queue<Tuple> q;
		q.push(start);
		QSet<Tuple> visited;

		while (!q.empty())
		{
			auto current = q.front();
			q.pop();
			if (visited.contains(current)) continue;

			auto nodes = current.valuesOfType<Model::Node*>();
			if (!matchSelf_ || start[1].second == current[0].second)
				result.add({{"ast", nodes[0]}});

			for (const auto& neighbor : neighbors(current))
				if (!visited.contains(neighbor))
					q.push(neighbor);

			visited.insert(current);
		}
	}
	return result;
}

std::vector<Tuple> CanReach::neighbors(const Tuple& t)
{
	std::vector<Tuple> neighbors;
	for (const auto& relation : relationTuples_)
		if (t[0].second == relation[1].second)
			neighbors.push_back(relation);
	return neighbors;
}

}
