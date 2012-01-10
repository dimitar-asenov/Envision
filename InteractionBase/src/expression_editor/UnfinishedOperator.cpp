/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * UnfinishedOperator.cpp
 *
 *  Created on: Jan 10, 2012
 *      Author: Dimitar Asenov
 */

#include "expression_editor/UnfinishedOperator.h"

#include "expression_editor/OperatorDescriptor.h"
#include "expression_editor/Empty.h"

namespace InteractionBase {

UnfinishedOperator::UnfinishedOperator(OperatorDescriptor* descriptor, Operator* parent)
	: Operator(descriptor, parent), num_complete_(0)
{
	for (int i = 0; i<descriptor->numOperands(); ++i)
		append(new Empty());
}

bool UnfinishedOperator::isComplete()
{
	return num_complete_ == descriptor()->signature().size();
}


QString UnfinishedOperator::next()
{
	return descriptor()->signature().at(num_complete_);
}

void UnfinishedOperator::addNext(Expression* e)
{
	if ( next() != "expr" && next() != "id")
	{
		++num_complete_;
		return;
	}

	int child_index = 0;
	for (int i = 0; i<num_complete_; ++i)
	{
		if (descriptor()->signature().at(i) == "expr" || descriptor()->signature().at(i) == "id")
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
		if (descriptor()->signature().at(i) != "expr" && descriptor()->signature().at(i) != "id")
			text += descriptor()->signature().at(i);
		else
			text += at(operand_index++)->renderText();

	return text;
}

Operator* UnfinishedOperator::createFinished()
{
	OperatorDescriptor* desc = descriptor()->isTransient() ? new OperatorDescriptor(*descriptor()) : descriptor();
	Operator* op = new Operator(desc, nullptr);
	for(int i = 0; i<size(); ++i)
	{
		op->append(first(true));
		append( new Empty() );
	}

	num_complete_ = 0;

	return op;
}

} /* namespace InteractionBase */
