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

#include "QueryBuilder.h"

#include "../queries/CompositeQuery.h"
#include "../queries/Query.h"
#include "../queries/AstQuery.h"
#include "../queries/AstNameFilter.h"
#include "../queries/SubstractNodesOperator.h"
#include "../queries/UnionOperator.h"
#include "../queries/NodePropertyAdder.h"
#include "../queries/ScriptQuery.h"

namespace InformationScripting {

static const QStringList OPEN_SCOPE_SYMBOL{"$", "\"", "{"};
static const QStringList CLOSE_SCOPE_SYMBOL{"$", "\"", "}"};

QueryBuilder& QueryBuilder::instance()
{
	static QueryBuilder instance;
	return instance;
}

Query* QueryBuilder::buildQueryFrom(const QString& text, Model::Node* target)
{
	target_ = target;
	Q_ASSERT(text.size());
	auto type = typeOf(text);
	if (Type::Operator == type)
		return parseOperator(text);
	else if (Type::Query == type)
		return parseQuery(text);
	else if (Type::List == type)
	{
		auto queries = parseList(text);
		auto result = new CompositeQuery();
		for (int i = 0; i < queries.size(); ++i)
		{
			result->connectInput(i, queries[i]);
			result->connectToOutput(queries[i], i);
		}
		return result;
	}
	Q_ASSERT(false);
	return nullptr;
}

void QueryBuilder::registerQueryConstructor(const QString& command, QueryBuilder::QueryConstructor constructor)
{
	constructors_[command] = constructor;
}

QueryBuilder::QueryBuilder()
{
	// TODO find a better place to register queries:
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
		return new ScriptQuery(QString("../InformationScripting/test/scripts/%1.py").arg(args[0]));
	});
}

QueryBuilder::Type QueryBuilder::typeOf(const QString& text)
{
	int index = OPEN_SCOPE_SYMBOL.indexOf(text[0]);
	Q_ASSERT(index >= 0);
	return static_cast<Type>(index);
}

QPair<QStringList, QList<QChar>> QueryBuilder::split(const QString& text, const QList<QChar>& splitChars)
{
	QPair<QStringList, QList<QChar>> result;
	QString currentString;
	QVector<int> openScopes(3, 0);
	for (int i = 1; i < text.size() - 1; ++i)
	{
		QChar currentChar = text.at(i);
		int openIndex = OPEN_SCOPE_SYMBOL.indexOf(currentChar);
		if (openIndex >= 0) ++openScopes[openIndex];
		int closeIndex = CLOSE_SCOPE_SYMBOL.indexOf(currentChar);
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

Query* QueryBuilder::parseQuery(const QString& text)
{
	Q_ASSERT(typeOf(text) == Type::Query);
	QStringList data = text.mid(1, text.size()-2).split(" ", QString::SkipEmptyParts);
	Q_ASSERT(data.size());
	QString command = data[0];
	auto constructor = constructors_[command];
	if (!constructor && (data.size() > 2 && data[1] == "="))
	{
		// TODO we need some way to specify a condition on the node.
		// Or eventually decide that we don't allow condition in the property adder
		return new NodePropertyAdder(data[0], data[2]);
	}
	// TODO this should only be an error presented to the user:
	Q_ASSERT(constructor);
	return constructor(target_, data.mid(1));
}

QList<Query*> QueryBuilder::parseList(const QString& text)
{
	Q_ASSERT(text.size());
	QStringList parts = split(text, {','}).first;
	QList<Query*> result;
	for (QString part : parts)
	{
		auto type = typeOf(part);
		if (Type::Operator == type)
			result.push_back(parseOperator(part, true));
		else if (Type::Query == type)
			result.push_back(parseQuery(part));
		else
			Q_ASSERT(false); // TODO error for user
	}
	return result;
}

Query* QueryBuilder::parseOperator(const QString& text, bool connectInput)
{
	// TODO it might be better to be able to register operators.
	// But that need some additional information on how they are used, thus we currently hardcode the operators.
	auto splitted = split(text, {'|', '-', 'U'});
	auto parts = splitted.first;
	auto operators = splitted.second;
	Q_ASSERT(parts.size()); // TODO error for user
	CompositeQuery* composite = new CompositeQuery();
	auto previousQueries = parseOperatorPart(parts[0]);
	if (connectInput)
	{
		for (int i = 0; i < previousQueries.size(); ++i)
			composite->connectInput(i, previousQueries[i]);
	}

	for (int i = 1; i < parts.size(); ++i)
	{
		auto currentQueries = parseOperatorPart(parts[i]);
		// If left and right are both lists we don't know how to map things:
		Q_ASSERT(!(previousQueries.size() > 1 && currentQueries.size() > 1)); // TODO error for user
		if (previousQueries.size() == 1 && currentQueries.size() == 1)
		{
			if (operators[i-1] == '|')
				composite->connectQuery(previousQueries[0], currentQueries[0]);
			else if (operators[i-1] == '-')
			{
				// TODO maybe minus should look like a union op, i.e. multiple input single output?
				auto minus = new SubstractNodesOperator();
				composite->connectQuery(previousQueries[0], minus);
				composite->connectQuery(currentQueries[0], 0, minus, 1);
				currentQueries = {minus};
			}
		}
		else if (previousQueries.size() > currentQueries.size())
		{
			auto unionOp = new UnionOperator();
			for (int i = 0; i < previousQueries.size(); ++i)
				composite->connectQuery(previousQueries[i], 0, unionOp, i);
			composite->connectQuery(unionOp, currentQueries[0]);
		}
		else
		{
			for (auto currentQuery : currentQueries)
				composite->connectQuery(previousQueries[0], currentQuery);
		}
		previousQueries = currentQueries;
	}
	// It might be that we have an operator at the end
	if (parts.size() > operators.size() || operators.last() == '|')
	{
		for (int i = 0; i < previousQueries.size(); ++i)
			composite->connectToOutput(previousQueries[i], i);
	}
	else if (operators.last() == 'U')
	{
		auto unionOp = new UnionOperator();
		for (int i = 0; i < previousQueries.size(); ++i)
			composite->connectQuery(previousQueries[i], 0, unionOp, i);
		composite->connectToOutput(unionOp);
	}
	else // No other case allowed
		Q_ASSERT(false); // TODO error for user
	return composite;
}

QList<Query*> QueryBuilder::parseOperatorPart(const QString& text)
{
	auto type = typeOf(text);
	if (Type::Query == type) return {parseQuery(text)};
	else if (Type::List == type) return parseList(text);
	Q_ASSERT(false);
}

} /* namespace InformationScripting  */
