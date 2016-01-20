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

#include "../nodes/CompositeQueryNode.h"
#include "../nodes/OperatorQueryNode.h"
#include "../nodes/CommandNode.h"

namespace InformationScripting {

constexpr QChar QueryParser::LIST_LEFT;
constexpr QChar QueryParser::LIST_RIGHT;
constexpr QChar QueryParser::LIST_DELIM;
constexpr QChar QueryParser::OP_PIPE;
constexpr QChar QueryParser::OP_MINUS_POSTFIX;

QueryNode* QueryParser::parse(const QString& queryString)
{
	int index = 0;
	auto query = parseAny(queryString, index);
	Q_ASSERT(index == queryString.length());
	changeToLeftToRightOrder(query);
	return query;
}

QueryNode* QueryParser::parseAny(const QString& queryString, int& index)
{
	if (index >= queryString.length()) return new CommandNode{};

	// Find name and arguments of the command
	QString commandString;
	QChar ch;
	while (index < queryString.length() )
	{
		ch = queryString[index];
		if (ch == LIST_DELIM || ch == OP_PIPE || ch == LIST_LEFT || ch == LIST_RIGHT) break;
		else ++index;

		commandString += ch;
	}

	Q_ASSERT( commandString.isEmpty() || ch != LIST_LEFT);
	QueryNode* query = nullptr;

	if (ch == LIST_LEFT)
	{
		query = parseList(queryString, index); // Create list
		if (index < queryString.length())
			ch = queryString[index];
	}
	else
	{
		// Create the command. Note that it could be an empty string.
		auto parts = quoteAwareSplit(commandString);
		if (parts.isEmpty())
			query = new CommandNode{""};
		else
		{
			auto commandString = parts.takeFirst();
			while (commandString.trimmed().isEmpty() && !parts.isEmpty())
				commandString = parts.takeFirst();
			auto command = new CommandNode{commandString};
			for (auto part : parts)
				command->arguments()->append(new CommandArgument{part});
			query = command;
		}
	}

	if (ch == LIST_DELIM || ch == LIST_RIGHT || index >= queryString.length()) return query;

	// We must have an operator, create it and keep parsing.
	Q_ASSERT(ch == OP_PIPE);
	auto op = new OperatorQueryNode{};
	if (index + 1 < queryString.size() && queryString[index + 1] == OP_MINUS_POSTFIX)
	{
		op->setOp(OperatorQueryNode::Substract);
		++index;
	}
	else
		op->setOp(OperatorQueryNode::Pipe);
	op->setLeft(query);
	op->setRight(parseAny(queryString, ++index));
	return op;
}

CompositeQueryNode* QueryParser::parseList(const QString& queryString, int& index)
{
	auto composite = new CompositeQueryNode{};
	QChar ch = LIST_DELIM;
	while (ch == LIST_DELIM)
	{
		composite->queries()->append( parseAny(queryString, ++index) );
		Q_ASSERT(index < queryString.length());
		ch = queryString[index];
	}
	Q_ASSERT(queryString[index] == LIST_RIGHT);
	++index;
	return composite;
}

void QueryParser::changeToLeftToRightOrder(QueryNode*& top)
{
	// By default the parser parses a|b|c as a|(b|c) but we want it to be (a|b)|c.
	// Since we always get a tree that grows down to the right,
	// we can easily transform it to a tree which grows down left:
	auto op = DCast<OperatorQueryNode>(top);
	while (op) {
		auto rightOp = DCast<OperatorQueryNode>(op->right());
		if (!rightOp)
			return;

		top = rightOp;

		auto rightLeftNode = rightOp->left();
		rightLeftNode->setParent(nullptr);

		op->setRight(rightLeftNode);

		rightOp->setLeft(op);

		op = rightOp;
	}
}

QStringList QueryParser::quoteAwareSplit(const QString& input)
{
	// TODO we should handle quote escaping as well. Note that this is a bit more involved:
	// Since the parsing is executed whenever a keypress occurs we can not just remove the escape character here.
	// But once a query is executed the string should not contain any escapes.
	QStringList parts;
	bool inQuote = false;
	QString currentPart;
	for (const QChar c : input)
	{
		if (c == ' ' && !inQuote)
		{
			parts << currentPart;
			currentPart.clear();
		}
		else
		{
			currentPart.append(c);
			if (c == '"')
				inQuote = !inQuote;
		}
	}
	parts << currentPart;
	return parts;
}

}
