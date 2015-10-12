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

#include "QueryNodeBuilder.h"

#include "ModelBase/src/nodes/Character.h"

#include "../nodes/CommandNode.h"
#include "../nodes/CompositeQueryNode.h"
#include "../nodes/OperatorQueryNode.h"
#include "../nodes/EmptyQueryNode.h"
#include "../nodes/UnfinishedQueryNode.h"
#include "../nodes/ErrorQueryNode.h"

#include "InteractionBase/src/expression_editor/Value.h"
#include "InteractionBase/src/expression_editor/Operator.h"
#include "InteractionBase/src/expression_editor/UnfinishedOperator.h"
#include "InteractionBase/src/expression_editor/OperatorDescriptor.h"
#include "InteractionBase/src/expression_editor/ErrorDescriptor.h"
#include "InteractionBase/src/expression_editor/ExpressionEditor.h"
#include "InteractionBase/src/expression_editor/Expression.h"

#include "../interaction/QueryOperatorDescriptorList.h"

#include "../interaction/QueryOperatorDescriptor.h"

namespace InformationScripting {

QueryNode* QueryNodeBuilder::parse(const QString& text)
{
	static Interaction::ExpressionEditor editor;
	editor.setOperatorDescriptors(QueryOperatorDescriptorList::instance());
	QueryNodeBuilder builder;
	return builder.buildQuery(editor.parse(text));
}

QueryNode* QueryNodeBuilder::buildQuery(Interaction::Expression* expression)
{
	expression->accept(this);
	auto q = query_;
	query_ = nullptr;
	return q;
}

void QueryNodeBuilder::visit(Interaction::Empty*)
{
	query_ = new EmptyQueryNode();
}

void QueryNodeBuilder::visit(Interaction::Value* val)
{
	if (val->text().isEmpty()) Q_ASSERT(false);
	query_ = new CommandNode(val->text());
}

void QueryNodeBuilder::visit(Interaction::Operator* op)
{
	qDebug() << "operator";
	if (dynamic_cast<Interaction::ErrorDescriptor*>(op->descriptor()))
		createErrorQuery(op);
	else
	{
		QList<QueryNode*> operands;
		for (auto q : op->operands())
		{
			q->accept(this);
			operands.append(query_);
		}
		auto descriptor = static_cast<QueryOperatorDescriptor*>(op->descriptor());
		query_ = descriptor->create(operands);
	}
}

void QueryNodeBuilder::visit(Interaction::UnfinishedOperator* unfinished)
{
	qDebug() << "unfinished";


	QString lastDelimiter;
	auto unf = new UnfinishedQueryNode();
	int operand_index = 0;
	for (int i = 0; i <unfinished->numComplete(); ++i)
	{
		QString current = unfinished->descriptor()->signature().at(i);

		if (!Interaction::OperatorDescriptor::isDelimiter(current))
		{
			// If there are two expressions next to each other without an operator in between, put a space.
			if (lastDelimiter.isEmpty() && operand_index > 0) lastDelimiter = ' ';

			unf->delimiters()->append(new Model::Text(lastDelimiter));
			lastDelimiter.clear();
			unfinished->at(operand_index)->accept(this);
			qDebug() << "query" << query_->typeName();
			++operand_index;
			unf->operands()->append(query_);
		}
		else
			lastDelimiter += current;
	}
	unf->delimiters()->append(new Model::Text(lastDelimiter)); // append the postfix, even if empty
	query_ = unf;
}

void QueryNodeBuilder::createErrorQuery(Interaction::Operator* op)
{
	auto error = new ErrorQueryNode();
	auto desc = dynamic_cast<Interaction::ErrorDescriptor*>(op->descriptor());
	error->setPrefix(desc->errorPrefix());
	error->setPostfix(desc->errorPostfix());

	op->at(0)->accept(this);
	error->setArg(query_);

	query_ = error;
}

} /* namespace InformationScripting  */
