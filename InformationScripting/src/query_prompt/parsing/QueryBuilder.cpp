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

#include "../../queries/CompositeQuery.h"
#include "../../queries/PassthroughQuery.h"
#include "../../queries/Yield.h"
#include "../../queries/SubstractOperator.h"
#include "../../queries/UnionOperator.h"
#include "../../query_framework/QueryRegistry.h"
#include "../../query_framework/QueryParsingException.h"

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

std::vector<std::unique_ptr<Query>> QueryBuilder::visitCommand(QueryBuilder* self, CommandNode* command)
{
	std::vector<std::unique_ptr<Query>> result;
	auto cmd = command->name();
	if (cmd.isEmpty())
	{
		result.emplace_back(std::unique_ptr<Query>{new PassthroughQuery{}});
		return result;
	}
	QStringList args;
	for (auto arg : *command->arguments())
		if (auto argNode = DCast<CommandArgument>(arg))
			args << argNode->argument();
	if (auto q = QueryRegistry::instance().buildQuery(cmd, self->target_, args, self->executor_))
	{
		result.emplace_back(std::move(q));
		return result;
	}
	throw QueryParsingException{QString{"%1 is not a valid command"}.arg(cmd)};
}

std::vector<std::unique_ptr<Query>> QueryBuilder::visitList(QueryBuilder* self, CompositeQueryNode* list)
{
	std::vector<std::unique_ptr<Query>> result;
	auto composite = std::make_unique<CompositeQuery>();
	for (int i = 0; i < list->queries()->size(); ++i)
	{
		// NOTE: subqueries with multiple inputs/outputs are not supported:
		auto queries = self->visit(list->queries()->at(i));
		if (queries.size() > 1)
			throw QueryParsingException{"yield in a list is not allowed"};
		auto query = composite->addQuery(std::move(queries[0]));
		composite->connectInput(i, query);
		composite->connectToOutput(0, query, i);
	}
	result.emplace_back(std::unique_ptr<Query>(composite.release()));
	return result;
}

std::vector<std::unique_ptr<Query>> QueryBuilder::visitOperator(QueryBuilder* self, OperatorQueryNode* op)
{
	std::vector<std::unique_ptr<Query>> result;

	auto composite = std::make_unique<CompositeQuery>();
	auto leftQueries = self->visit(op->left());

	auto rightQueries = self->visit(op->right());
	// By the order of the tree we should always only have one right side:
	Q_ASSERT(rightQueries.size() == 1);
	if (dynamic_cast<Yield*>(rightQueries[0].get()) != nullptr)
	{
		result = std::move(leftQueries);
		result.emplace_back(std::move(rightQueries[0]));
		return result;
	}

	auto lastLeftQuery = std::move(leftQueries.back());
	leftQueries.pop_back();
	if (dynamic_cast<Yield*>(lastLeftQuery.get()) != nullptr)
	{
		result = std::move(leftQueries);
		result.emplace_back(std::move(rightQueries[0]));
		return result;
	}

	auto left = composite->addQuery(std::move(lastLeftQuery));
	auto leftComposite = dynamic_cast<CompositeQuery*>(left);

	auto right = composite->addQuery(std::move(rightQueries[0]));
	auto rightComposite = dynamic_cast<CompositeQuery*>(right);

	if (leftComposite && rightComposite)
	{
		if (leftComposite->outputCount() == rightComposite->inputCount())
		{
			for (int i = 0; i < leftComposite->outputCount(); ++i)
				composite->connectQuery(leftComposite, i, rightComposite, i);
		}
		else if (leftComposite->outputCount() == 1 && rightComposite->inputCount() > 1)
		{
			connectAsSplit(composite.get(), left, rightComposite);
		}
		else
		{
			if (rightComposite->inputCount() > 1)
				throw QueryParsingException{"Connecting lists with differing sizes is not allowed."};
			connectAsUnion(composite.get(), leftComposite, right, op);
		}
	}
	else if (leftComposite)
	{
		if (leftComposite->outputCount() > 1)
			connectAsUnion(composite.get(), leftComposite, right, op);
		else
			composite->connectQuery(left, right);
	}
	else if (rightComposite)
	{
		connectAsSplit(composite.get(), left, rightComposite);
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
		composite->connectToOutput(o, right, o);

	result = std::move(leftQueries);
	result.emplace_back(std::unique_ptr<Query>(composite.release()));
	return result;
}

void QueryBuilder::connectQueriesWith(CompositeQuery* composite, CompositeQuery* queries,
												  Query* connectionQuery, Query* outputQuery)
{
	for (int i = 0; i < queries->outputCount(); ++i)
		composite->connectQuery(queries, i, connectionQuery, i);
	if (outputQuery)
		composite->connectQuery(connectionQuery, outputQuery);
}

void QueryBuilder::connectAsUnion(CompositeQuery* composite, CompositeQuery* left, Query* right, OperatorQueryNode* op)
{
	Query* unionQuery = nullptr;
	if (op->op() == OperatorQueryNode::OperatorTypes::Pipe)
		unionQuery = composite->addQuery(std::unique_ptr<Query>(new UnionOperator{}));
	else if (op->op() == OperatorQueryNode::OperatorTypes::Substract)
		unionQuery = composite->addQuery(std::unique_ptr<Query>(new SubstractOperator{}));
	else
		Q_ASSERT(false); // No other case possible
	connectQueriesWith(composite, left, unionQuery, right);
}

void QueryBuilder::connectAsSplit(CompositeQuery* composite, Query* left, CompositeQuery* right)
{
	for (int i = 0; i < right->inputCount(); ++i)
		composite->connectQuery(left, 0, right, i);
}

}
