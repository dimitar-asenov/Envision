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

#include "../nodes/CommandNode.h"
#include "../nodes/CompositeQueryNode.h"
#include "../nodes/OperatorQueryNode.h"

#include "../queries/CompositeQuery.h"
#include "../queries/QueryRegistry.h"
#include "../queries/SubstractOperator.h"
#include "../queries/UnionOperator.h"

#include "../parsing/QueryParsingException.h"

namespace InformationScripting {

QueryBuilder::QueryBuilder(Model::Node* target, QueryExecutor* executor)
	: executor_{executor}, target_{target}
{}

void QueryBuilder::init()
{
	addType<CommandNode>(visitCommand);
	addType<CompositeQueryNode>(visitList);
	addType<OperatorQueryNode>(visitOperator);
}

std::unique_ptr<Query> QueryBuilder::visitCommand(QueryBuilder* self, CommandNode* command)
{
	auto text = command->name();
	if (text.isEmpty())
		throw QueryParsingException("Empty command is not allowed");
	auto parts = text.split(" ", QString::SplitBehavior::SkipEmptyParts);
	auto cmd = parts.takeFirst();
	if (auto q = QueryRegistry::instance().buildQuery(cmd, self->target_, parts, self->executor_))
		return q;
	throw QueryParsingException(QString("%1 is not a valid command").arg(cmd));
}

std::unique_ptr<Query> QueryBuilder::visitList(QueryBuilder* self, CompositeQueryNode* list)
{
	auto composite = std::make_unique<CompositeQuery>();
	for (int i = 0; i < list->queries()->size(); ++i)
	{
		auto query = composite->addQuery(self->visit(list->queries()->at(i)));
		composite->connectInput(i, query);
		composite->connectToOutput(query, i);
	}
	return std::unique_ptr<Query>(composite.release());
}

std::unique_ptr<Query> QueryBuilder::visitOperator(QueryBuilder* self, OperatorQueryNode* op)
{
	auto composite = std::make_unique<CompositeQuery>();

	auto left = composite->addQuery(self->visit(op->left()));
	auto leftComposite = dynamic_cast<CompositeQuery*>(left);
	auto right = composite->addQuery(self->visit(op->right()));
	auto rightComposite = dynamic_cast<CompositeQuery*>(right);

	if (leftComposite && rightComposite)
	{
		Q_ASSERT(leftComposite->outputCount() == rightComposite->inputCount());
		for (int i = 0; i < leftComposite->outputCount(); ++i)
			composite->connectQuery(leftComposite, i, rightComposite, i);
	}
	else if (leftComposite)
	{
		// union
		Query* unionQuery = nullptr;
		if (op->op() == OperatorQueryNode::OperatorTypes::Pipe)
			unionQuery = composite->addQuery(std::unique_ptr<Query>(new UnionOperator()));
		else if (op->op() == OperatorQueryNode::OperatorTypes::Substract)
			unionQuery = composite->addQuery(std::unique_ptr<Query>(new SubstractOperator()));
		else
			Q_ASSERT(false); // No other case possible
		connectQueriesWith(composite.get(), leftComposite, unionQuery, right);
	}
	else if (rightComposite)
	{
		// split
		for (int i = 0; i < rightComposite->inputCount(); ++i)
			composite->connectQuery(left, 0, rightComposite, i);
	}
	else
	{
		if (op->op() == OperatorQueryNode::OperatorTypes::Pipe)
			composite->connectQuery(left, right);
		else if (op->op() == OperatorQueryNode::OperatorTypes::Substract)
		{
			auto minus = composite->addQuery(std::unique_ptr<Query>(new SubstractOperator));
			composite->connectQuery(left, minus);
			composite->connectQuery(right, 0, minus, 1);
			right = minus;
		}
	}
	int inputCount = leftComposite ? leftComposite->inputCount() : 1;
	int outputCount = rightComposite ? rightComposite->outputCount() : 1;
	for (int i = 0; i < inputCount; ++i)
		composite->connectInput(i, left, i);
	for (int o = 0; o < outputCount; ++o)
		composite->connectToOutput(right, o);
	return std::unique_ptr<Query>(composite.release());
}

void QueryBuilder::connectQueriesWith(CompositeQuery* composite, CompositeQuery* queries,
												  Query* connectionQuery, Query* outputQuery)
{
	for (int i = 0; i < queries->outputCount(); ++i)
		composite->connectQuery(queries, i, connectionQuery, i);
	if (outputQuery)
		composite->connectQuery(connectionQuery, outputQuery);
}

} /* namespace InformationScripting */
