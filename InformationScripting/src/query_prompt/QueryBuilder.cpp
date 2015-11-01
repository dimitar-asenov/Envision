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
#include "../queries/UnionOperator.h"

namespace InformationScripting {

QueryBuilder::QueryBuilder(Model::Node* target, QueryExecutor* executor)
	: executor_{executor}, target_{target}
{}

void QueryBuilder::visit(CommandNode* command)
{
	auto text = command->name();
	// TODO throw
	Q_ASSERT(!text.isEmpty());
	auto parts = text.split(" ", QString::SplitBehavior::SkipEmptyParts);
	auto cmd = parts.takeFirst();
	// TODO throw on nullptr, this might be problematic since we might have some memory already allocated.
	query_ = QueryRegistry::instance().buildQuery(cmd, target_, parts, executor_);
}

void QueryBuilder::visit(CompositeQueryNode* list)
{
	auto composite = new CompositeQuery();
	for (int i = 0; i < list->queries()->size(); ++i)
	{
		list->queries()->at(i)->accept(this);
		composite->connectInput(i, query_);
		composite->connectToOutput(query_, i);
	}
	query_ = composite;
}

void QueryBuilder::visit(OperatorQueryNode* op)
{
	Q_ASSERT(op->op() == OperatorQueryNode::OperatorTypes::Pipe);

	op->left()->accept(this);
	auto left = query_;
	auto leftComposite = dynamic_cast<CompositeQuery*>(left);
	op->right()->accept(this);
	auto right = query_;
	auto rightComposite = dynamic_cast<CompositeQuery*>(right);

	auto composite = new CompositeQuery();
	if (leftComposite && rightComposite)
	{
		Q_ASSERT(leftComposite->outputCount() == rightComposite->inputCount());
		for (int i = 0; i < leftComposite->outputCount(); ++i)
			composite->connectQuery(leftComposite, i, rightComposite, i);
	}
	else if (leftComposite)
	{
		// union
		connectQueriesWith(composite, leftComposite, new UnionOperator(), right);
	}
	else if (rightComposite)
	{
		// split
		for (int i = 0; i < rightComposite->inputCount(); ++i)
			composite->connectQuery(left, 0, rightComposite, i);
	}
	else
	{
		composite->connectQuery(left, right);
	}
	int inputCount = leftComposite ? leftComposite->inputCount() : 1;
	int outputCount = rightComposite ? rightComposite->outputCount() : 1;
	for (int i = 0; i < inputCount; ++i)
		composite->connectInput(i, left, i);
	for (int o = 0; o < outputCount; ++o)
		composite->connectToOutput(right, o);
	query_ = composite;
}

void QueryBuilder::visit(EmptyQueryNode*)
{
	// TODO: throw
	Q_ASSERT(false);
	query_ = nullptr;
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
