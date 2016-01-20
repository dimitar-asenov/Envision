/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "UnfinishedOperator.h"

#include "OperatorDescriptor.h"
#include "Empty.h"
#include "ExpressionVisitor.h"
#include "ExpressionTreeUtils.h"

namespace Interaction {

UnfinishedOperator::UnfinishedOperator(OperatorDescriptor* descriptor, Operator* parent)
	: Operator{descriptor, parent}, num_complete_{0}
{
	for (int i = 0; i<descriptor->numOperands(); ++i)
		append(new Empty{});
}

bool UnfinishedOperator::isComplete()
{
	return num_complete_ == descriptor()->signature().size();
}

void UnfinishedOperator::accept(ExpressionVisitor* visitor)
{
	visitor->visit(this);
}

QString UnfinishedOperator::next()
{
	return descriptor()->signature().at(num_complete_);
}

void UnfinishedOperator::addNext(Expression* e)
{
	if ( descriptor()->isDelimiter(num_complete_) )
	{
		++num_complete_;
		return;
	}

	Q_ASSERT(e);
	int child_index = 0;
	for (int i = 0; i<num_complete_; ++i)
	{
		if ( !descriptor()->isDelimiter(i))
			child_index++;
	}

	delete at(child_index, true);
	insert(e, child_index);
	++num_complete_;
}

QString UnfinishedOperator::renderText()
{
	QString text;
	int operand_index = 0;
	for (int i=0; i < num_complete_; ++i)
		if ( descriptor()->isDelimiter(i) )
			text += descriptor()->signature().at(i);
		else
			text += at(operand_index++)->renderText();

	return text;
}

Operator* UnfinishedOperator::createFinished()
{
	if (descriptor()->isTransient()) setDoNotDeleteTransientDescriptor(true);

	Operator* op = new Operator{descriptor(), nullptr};
	for (int i = 0; i<size(); ++i)
	{
		op->append(first(true));
		append( new Empty{} );
	}

	num_complete_ = 0;

	return op;
}

UnfinishedOperator* UnfinishedOperator::replaceFinishedWithUnfinished(Expression*& top, Operator* op)
{
	auto unf = new UnfinishedOperator{op->descriptor()};

	int operandIndex = 0;
	for (int i = 0; i< op->descriptor()->signature().size(); ++i)
	{
		if ( !op->descriptor()->isDelimiter(i) )
		{
			auto e = op->replaceOperand(op->operands().at(operandIndex), new Empty{});
			unf->addNext(e);
			++operandIndex;
		}
		else unf->addNext();
	}

	delete ExpressionTreeUtils::replace(top, op, unf );
	return unf;
}

}
