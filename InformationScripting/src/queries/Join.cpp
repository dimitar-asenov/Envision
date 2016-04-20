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

#include "Join.h"

#include "../query_framework/QueryRegistry.h"

namespace InformationScripting {

const QStringList Join::VALUE_ARGUMENT_NAMES{"v", "values"};
const QStringList Join::AS_ARGUMENT_NAMES{"a", "as"};
const QStringList Join::ON_ARGUMENT_NAMES{"o", "on"};

Optional<TupleSet> Join::executeLinear(TupleSet input)
{
	QList<TaggedValue> values;
	QString tag1;
	QString tag2;
	std::pair<TaggedValue, TaggedValue> joinOn;

	const QRegularExpression onRegex{"(\\w+)\\.(\\w+)=(\\w+)\\.(\\w+)"};
	auto onMatches = onRegex.match(arguments_.argument(ON_ARGUMENT_NAMES[1]));
	if (onMatches.hasMatch())
	{
		tag1 = onMatches.captured(1);
		tag2 = onMatches.captured(3);
		joinOn = {{tag1, onMatches.captured(2)}, {tag2, onMatches.captured(4)}};
	}

	const QRegularExpression valueMatch{"((\\w+)\\.)?(\\w+)"};
	auto valueMatchIt = valueMatch.globalMatch(arguments_.argument(VALUE_ARGUMENT_NAMES[1]));
	while (valueMatchIt.hasNext())
	{
		auto match = valueMatchIt.next();
		if (!match.hasMatch()) return {"join values need the following format: tag.value"};
		auto tag = match.captured(2);
		auto value = match.captured(3);

		values.push_back({tag, value});

		if (!tag.isEmpty())
		{
			if (tag1.isNull() || tag1 == tag) tag1 = tag;
			else if (tag1 != tag && tag2.isNull()) tag2 = tag;
			else if (tag2 != tag) return {"join values can only be from 2 different tuples"};
		}
	}

	if (tag1.isNull() || tag2.isNull())
		return {"join only works on 2 different tuples"};

	// By default join on id
	if (joinOn.first.first.isEmpty()) joinOn = {{tag1, "id"}, {tag2, "id"}};

	auto tag1Tuples = input.tuples(tag1);
	auto tag2Tuples = input.tuples(tag2);
	if (tag1Tuples.isEmpty() || tag2Tuples.isEmpty())
		return {input, "Not enough input for join"};

	if (tag1 != joinOn.first.first)
		std::swap(joinOn.first, joinOn.second);

	auto id1Name = joinOn.first.second;
	auto id2Name = joinOn.second.second;

	for (const auto& tuple1 : tag1Tuples)
	{
		auto it = tuple1.find(id1Name);
		if (it == tuple1.end())
			return {QString{"Tuple %1 does not contain %2 which is required for join"}.arg(tag1, id1Name)};
		Property id1 = it->second;

		bool attributeNotFound = false;
		auto it2 = std::find_if(tag2Tuples.begin(), tag2Tuples.end(), [id1, &attributeNotFound, id2Name](const Tuple& t)
		{
			auto idIt = t.find(id2Name);
			if (idIt == t.end())
			{
				attributeNotFound = true;
				return false;
			}
			return idIt->second == id1;
		});

		if (attributeNotFound)
			return {QString{"Tuple %1 does not contain %2 which is required for join"}.arg(tag2, id2Name)};


		if (it2 != tag2Tuples.end())
		{
			// Found a match
			auto props1 = extractProperties(tuple1, values);
			if (props1.hasErrors()) return props1.errors()[0];
			auto props2 = extractProperties(*it2, values);
			if (props2.hasErrors()) return props2.errors()[0];
			input.add(Tuple{arguments_.argument(AS_ARGUMENT_NAMES[1]), props1.value() + props2.value()});
		}

	}
	return input;
}

void Join::registerDefaultQueries()
{
	QueryRegistry::registerQuery<Join>("join",
		std::vector<ArgumentRule>{{ArgumentRule::RequireAll, {{VALUE_ARGUMENT_NAMES[1]}, {AS_ARGUMENT_NAMES[1]}}}});
}

Join::Join(Model::Node* target, QStringList args, std::vector<ArgumentRule> argumentRules)
	: LinearQuery{target}, arguments_{{
	{AS_ARGUMENT_NAMES, "Name of the joined tuple",  AS_ARGUMENT_NAMES[1]},
	{ON_ARGUMENT_NAMES, "Name of the attributes to join on",  ON_ARGUMENT_NAMES[1]}
	}, {PositionalArgument{VALUE_ARGUMENT_NAMES[1], "Name of the attribute(s) that will be in the joined tuple"}},
args}
{
	for (const auto& rule : argumentRules)
		rule.check(arguments_);
}

Optional<QList<NamedProperty>> Join::extractProperties(const Tuple& t, const QList<std::pair<QString, QString>>& values)
{
	QList<NamedProperty> result;
	for (const auto& v : values)
	{
		if (v.first.isEmpty() || v.first == t.tag())
		{
			auto it = t.find(v.second);
			if (it == t.end() && v.first.isEmpty()) continue;
			else if (it == t.end()) return QString{"Tuple %1 does not have any value %2"}.arg(t.tag(), v.second);
			else result.push_back(*it);
		}
	}
	return result;
}

}
