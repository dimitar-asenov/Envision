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

#include "QueryParser.h"

#include "../queries/CompositeQuery.h"
#include "../queries/Query.h"
#include "../queries/SubstractOperator.h"
#include "../queries/UnionOperator.h"
#include "../queries/TopLevelQuery.h"

#include "../queries/QueryRegistry.h"

namespace InformationScripting {

const QStringList QueryParser::OPEN_SCOPE_SYMBOL{"$<", "\"<", "{<"};
const QStringList QueryParser::CLOSE_SCOPE_SYMBOL{">$", ">\"", ">}"};

QList<TopLevelQuery*> QueryParser::buildQueryFrom(const QString& text, Model::Node* target)
{
	QueryParser parser;
	parser.target_ = target;
	Q_ASSERT(text.size());
	// For now assume that we only have yield in operator parts:
	auto parts = text.split("|" + OPEN_SCOPE_SYMBOL[1] + "yield" + CLOSE_SCOPE_SYMBOL[1] + "|",
			QString::SkipEmptyParts);
	if (parts.size() > 1)
	{
		for (int i = 0; i < parts.size(); ++i)
		{
			if (i + 1 < parts.size()) parts[i].append(CLOSE_SCOPE_SYMBOL[0]);
			if (i > 0) parts[i].prepend(OPEN_SCOPE_SYMBOL[0]);
		}
	}
	QList<TopLevelQuery*> results;
	for (auto part : parts)
	{
		auto type = parser.typeOf(part);
		if (Type::Operator == type)
			results << new TopLevelQuery(parser.parseOperator(nullptr, part, true));
		else if (Type::Query == type)
			results << new TopLevelQuery(parser.parseQuery(nullptr, part));
		else if (Type::List == type)
		{
			auto result = new CompositeQuery(nullptr);
			auto queries = parser.parseList(result, part);
			for (int i = 0; i < queries.size(); ++i)
			{
				result->connectInput(i, queries[i]);
				result->connectToOutput(queries[i], i);
			}
			results << new TopLevelQuery(result);
		}
		else
			Q_ASSERT(false);
	}
	return results;
}

QueryParser::Type QueryParser::typeOf(const QString& text)
{
	int index = OPEN_SCOPE_SYMBOL.indexOf(text.mid(0, SCOPE_SYMBOL_LENGTH_));
	Q_ASSERT(index >= 0);
	return static_cast<Type>(index);
}

QPair<QStringList, QList<QChar>> QueryParser::split(const QString& text, const QList<QChar>& splitChars)
{
	QPair<QStringList, QList<QChar>> result;
	QString currentString;
	QVector<int> openScopes(OPEN_SCOPE_SYMBOL.length(), 0);
	for (int i = SCOPE_SYMBOL_LENGTH_; i < text.size() - SCOPE_SYMBOL_LENGTH_; ++i)
	{
		QChar currentChar = text.at(i);
		QString scopeSymbol = text.mid(i, SCOPE_SYMBOL_LENGTH_);
		int openIndex = OPEN_SCOPE_SYMBOL.indexOf(scopeSymbol);
		if (openIndex >= 0) ++openScopes[openIndex];
		int closeIndex = CLOSE_SCOPE_SYMBOL.indexOf(scopeSymbol);
		if (closeIndex >= 0) --openScopes[closeIndex];
		bool isInScope = std::all_of(openScopes.begin(), openScopes.end(), [](int i) { return i == 0;});
		if (isInScope && splitChars.contains(currentChar))
		{
			result.second << currentChar;
			if (!currentString.isEmpty()) result.first << currentString;
			currentString = "";
		}
		else
			currentString.append(currentChar);
	}
	if (!currentString.isEmpty()) result.first << currentString;

	return result;
}

Query* QueryParser::parseQuery(Query* parent, const QString& text)
{
	Q_ASSERT(typeOf(text) == Type::Query);
	QStringList data = text.mid(SCOPE_SYMBOL_LENGTH_,
										 text.size() - 2 * SCOPE_SYMBOL_LENGTH_).split(" ", QString::SkipEmptyParts);
	Q_ASSERT(data.size());
	QString command = data.takeFirst();
	auto q = QueryRegistry::instance().buildQuery(parent, command, target_, data);
	Q_ASSERT(q); // TODO this should be an error for the user.
	return q;
}

QList<Query*> QueryParser::parseList(Query* parent, const QString& text)
{
	Q_ASSERT(text.size());
	QStringList parts = split(text, {','}).first;
	QList<Query*> result;
	for (QString part : parts)
	{
		auto type = typeOf(part);
		if (Type::Operator == type)
			result.push_back(parseOperator(parent, part, true));
		else if (Type::Query == type)
			result.push_back(parseQuery(parent, part));
		else
			Q_ASSERT(false); // TODO error for user
	}
	return result;
}

Query* QueryParser::parseOperator(Query* parent, const QString& text, bool connectInput)
{
	// TODO it might be better to be able to register operators.
	// But that need some additional information on how they are used, thus we currently hardcode the operators.
	auto splitText = split(text, {'|', '-', 'U'});
	auto parts = splitText.first;
	auto operators = splitText.second;
	Q_ASSERT(parts.size()); // TODO error for user
	CompositeQuery* composite = new CompositeQuery(parent);
	auto previousQueries = parseOperatorPart(composite, parts[0]);
	if (connectInput)
	{
		for (int i = 0; i < previousQueries.size(); ++i)
			composite->connectInput(i, previousQueries[i]);
	}

	for (int i = 1; i < parts.size(); ++i)
	{
		auto currentQueries = parseOperatorPart(composite, parts[i]);
		auto currentOperator = operators[i-1];
		// If left and right are both lists we don't know how to map things:
		Q_ASSERT(!(previousQueries.size() > 1 && currentQueries.size() > 1)); // TODO error for user
		if (previousQueries.size() == 1 && currentQueries.size() == 1)
		{
			if (currentOperator == '|')
				composite->connectQuery(previousQueries[0], currentQueries[0]);
			else if (currentOperator == '-')
			{
				auto minus = new SubstractOperator(composite);
				connectQueriesWith(composite, {previousQueries[0], currentQueries[0]}, minus);
				currentQueries = {minus};
			}
		}
		else if (previousQueries.size() > currentQueries.size())
		{
			Query* unionQuery = nullptr;
			if (operators[i-1] == '-') unionQuery = new SubstractOperator(composite);
			else unionQuery = new UnionOperator(composite);
			connectQueriesWith(composite, previousQueries, unionQuery, currentQueries[0]);
		}
		else
		{
			// Split the output from the previous query
			for (auto currentQuery : currentQueries)
				composite->connectQuery(previousQueries[0], currentQuery);
		}
		previousQueries = currentQueries;
	}

	if (parts.size() > operators.size() || operators.last() == '|')
		composite->connectToOutput(previousQueries);
	else
	{
		// We have an operator at the end
		auto lastOperator = operators.last();
		if (lastOperator == '-' || lastOperator == 'U')
		{
			Query* unionQuery = nullptr;
			if (lastOperator == '-') unionQuery = new SubstractOperator(composite);
			else unionQuery = new UnionOperator(composite);
			connectQueriesWith(composite, previousQueries, unionQuery);
			composite->connectToOutput(unionQuery);
		}
		else // No other case allowed
			Q_ASSERT(false); // TODO error for user
	}
	return composite;
}

QList<Query*> QueryParser::parseOperatorPart(Query* parent, const QString& text)
{
	auto type = typeOf(text);
	if (Type::Query == type) return {parseQuery(parent, text)};
	else if (Type::List == type) return parseList(parent, text);
	Q_ASSERT(false);
}

void QueryParser::connectQueriesWith(CompositeQuery* composite, const QList<Query*>& queries,
												  Query* connectionQuery, Query* outputQuery)
{
	for (int i = 0; i < queries.size(); ++i)
		composite->connectQuery(queries[i], 0, connectionQuery, i);
	if (outputQuery)
		composite->connectQuery(connectionQuery, outputQuery);
}

} /* namespace InformationScripting  */
