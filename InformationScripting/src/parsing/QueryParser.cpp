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

#include "ModelBase/src/nodes/Character.h"

#include "../nodes/CommandNode.h"
#include "../nodes/CompositeQueryNode.h"
#include "../nodes/OperatorNode.h"

namespace InformationScripting {

const QStringList QueryParser::OPEN_SCOPE_SYMBOL{"$<", "\"<", "{<"};
const QStringList QueryParser::CLOSE_SCOPE_SYMBOL{">$", ">\"", ">}"};

QueryParser& QueryParser::instance()
{
	static QueryParser instance;
	return instance;
}

QueryNode* QueryParser::parse(const QString& text)
{
	Q_ASSERT(text.size());
	auto type = typeOf(text);
	if (Type::Operator == type)
		return parseOperator(text);
	else if (Type::Command == type)
		return parseCommand(text);
	else if (Type::List == type)
		return parseList(text);
	Q_ASSERT(false);
	return nullptr;
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

CommandNode* QueryParser::parseCommand(const QString& text)
{
	Q_ASSERT(typeOf(text) == Type::Command);
	QStringList data = text.mid(SCOPE_SYMBOL_LENGTH_,
										 text.size() - 2 * SCOPE_SYMBOL_LENGTH_).split(" ", QString::SkipEmptyParts);
	Q_ASSERT(data.size()); // TODO empty node needed?
	QString command = data.takeFirst();
	auto commandNode = new CommandNode(command);
	for (const auto& arg : data)
		commandNode->arguments()->append(new CommandArgument(arg));
	return commandNode;
}

CompositeQueryNode *QueryParser::parseList(const QString& text)
{
	Q_ASSERT(text.size());
	QStringList parts = split(text, {','}).first;
	auto result = new CompositeQueryNode();
	for (QString part : parts)
	{
		auto type = typeOf(part);
		if (Type::Operator == type)
			result->queries()->append(parseOperator(part));
		else if (Type::Command == type)
			result->queries()->append(parseCommand(part));
		else
			Q_ASSERT(false); // TODO empty node?
	}
	return result;
}

OperatorNode* QueryParser::parseOperator(const QString& text)
{
	auto splitText = split(text, {'|', '-', 'U'});
	auto parts = splitText.first;
	auto operators = splitText.second;
	auto operatorNode = new OperatorNode();
	for (const auto& part : parts)
		operatorNode->operands()->append(parseOperatorPart(part));
	for (const auto& op : operators)
		operatorNode->operators()->append(new Model::Character(op));
	return operatorNode;
}

QueryNode* QueryParser::parseOperatorPart(const QString& text)
{
	auto type = typeOf(text);
	if (Type::Command == type) return parseCommand(text);
	else if (Type::List == type) return parseList(text);
	Q_ASSERT(false);
}

} /* namespace InformationScripting  */
