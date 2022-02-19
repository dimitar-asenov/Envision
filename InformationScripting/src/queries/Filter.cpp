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

#include "Filter.h"

#include "../query_framework/QueryRegistry.h"
#include "../query_framework/QueryParsingException.h"

#include "ModelBase/src/util/SymbolMatcher.h"

namespace InformationScripting {

const QStringList Filter::EXTRACT_ARGUMENT_NAMES{"e", "extract"};
const QStringList Filter::AS_ARGUMENT_NAMES{"as"};

Optional<TupleSet> Filter::executeLinear(TupleSet input)
{
	auto filterTag = arguments_.positionalArgument(0);

	auto tagParts = filterTag.split(".");
	QString tupleTag = tagParts[0];

	if (tagParts.size() < 2) {
		auto outputList = input.take(tupleTag);
		return TupleSet(QList<Tuple>{outputList.begin(), outputList.end()});
	}

	bool extractIsSet = arguments_.isArgumentSet(EXTRACT_ARGUMENT_NAMES[1]);

	if (!extractIsSet && arguments_.numPositionalArguments() < 2)
		return {"filter: when filtering by value, a value or the extract argument is required."};

	QString tupleValueTag = tagParts[1];

	if (extractIsSet)
		return extract(tupleTag, tupleValueTag, input);

	auto filterBy = removeOuterQuotes(arguments_.positionalArgument(1));

	TupleSet result;

	for (const auto& candidate : input.take(tupleTag))
	{
		auto it = candidate.find(tupleValueTag);
		if (it == candidate.end())
			return {QString{"Tuple %1 does not contain %2"}.arg(tupleTag, tupleValueTag)};
		Property value = it->second;
		if (!value.isConvertibleTo<QString>())
			return {"filter only works on string values"};
		QString valueString = value;
		auto matcher = Model::SymbolMatcher::guessMatcher(filterBy);
		if (matcher.matches(valueString))
			result.add(candidate);
	}
	return result;
}

void Filter::registerDefaultQueries()
{
	QueryRegistry::registerQuery<Filter>("filter");
}

Filter::Filter(Model::Node* target, QStringList args)
	: LinearQuery{target}, arguments_{
	{QCommandLineOption{EXTRACT_ARGUMENT_NAMES},
	 {AS_ARGUMENT_NAMES, "Tag to put the extracted value in", AS_ARGUMENT_NAMES[0]}
	},
	{PositionalArgument{"tag", "The tag to filter on"},
	 PositionalArgument{"by", "The value by which we should filter"}
	}, args}
{
	if (arguments_.numPositionalArguments() < 1)
		throw QueryParsingException{arguments_.queryName() + " Requires at least one arguments"};
}

Optional<TupleSet> Filter::extract(const QString& tag, const QString& value, TupleSet& input)
{
	TupleSet result;
	QString as = arguments_.argument(AS_ARGUMENT_NAMES[0]);
	if (as.isEmpty()) as = value;
	for (const auto& tuple : input.tuples(tag))
	{
		auto it = tuple.find(value);
		if (it == tuple.end())
			return {QString{"Tuple with tag %1 has no value with name %2"}.arg(tag, value)};
		result.add(Tuple{{{as, it->second}}});
	}
	return result;
}

QString Filter::removeOuterQuotes(const QString& from)
{
	QString result = from;
	if (result.startsWith("\""))
		result = result.mid(1);
	if (result.endsWith("\""))
		result = result.left(result.size() - 1);
	return result;
}

}
