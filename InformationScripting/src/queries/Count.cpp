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

#include "Count.h"

#include "../query_framework/QueryRegistry.h"

#include "../query_framework/QueryParsingException.h"

namespace InformationScripting {

Optional<TupleSet> Count::executeLinear(TupleSet input)
{
	auto countByTags = arguments_.positionalArgument(0).split(",");
	Q_ASSERT(!countByTags.isEmpty()); // This is checked by the argument rule

	QHash<Tuple, int> counts;
	if (countByTags[0].contains("."))
	{
		auto parts = countByTags[0].split(".");
		if (parts.size() > 2) return {"count: by argument can only have a single . (dot)"};
		QString tag = parts[0];
		bool allSame = std::all_of(countByTags.begin(), countByTags.end(), [tag](const QString& by) {
			return by.split(".")[0] == tag;
		});
		if (!allSame) return {"count: by argument values should all be from same tuple"};

		auto taggedTuples = input.tuples(tag);
		for (const auto& tuple : taggedTuples)
		{
			QList<NamedProperty> keyTuple;
			for (const auto& byTag : countByTags)
			{
				auto propName = byTag.split(".")[1];
				auto it = tuple.find(propName);
				if (it == tuple.end())
					return {QString{"Tuple tagged with %1 does not contain value with tag %2"}.arg(tuple.tag(), propName)};
				keyTuple.push_back(*it);
			}
			++counts[Tuple{keyTuple}];
		}

	}
	else
		return {"count: need to specify tag of tuple to extract values from"};

	for (auto it = counts.begin(); it != counts.end(); ++it)
	{
		Tuple countTuple({{"count", it.value()}});
		for (const auto& namedProperty : it.key())
			countTuple.add(namedProperty);
		input.add(countTuple);
	}
	return input;
}

void Count::registerDefaultQueries()
{
	QueryRegistry::registerQuery<Count>("count");
}

Count::Count(Model::Node* target, QStringList args)
	: LinearQuery{target}, arguments_{{
	PositionalArgument{"by", "Name of the attribute(s) that should be used to count on"}
	}, args}
{
	if (arguments_.numPositionalArguments() < 1)
		throw QueryParsingException{arguments_.queryName() + " Requires one argument"};
}


}
