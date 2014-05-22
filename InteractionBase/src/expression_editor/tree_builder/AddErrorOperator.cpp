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

#include "expression_editor/tree_builder/AddErrorOperator.h"

#include "expression_editor/tree_builder/ExpressionTreeBuilder.h"
#include "expression_editor/Operator.h"
#include "expression_editor/UnfinishedOperator.h"
#include "expression_editor/ErrorDescriptor.h"
#include "expression_editor/ExpressionTreeUtils.h"

namespace Interaction {

AddErrorOperator::AddErrorOperator(const QString& text) : text_(text)
{
}

void AddErrorOperator::perform(ExpressionTreeBuilder& tb)
{
	if (tb.left())
	{
		Operator* err = new Operator(new ErrorDescriptor("", text_));
		ExpressionTreeUtils::replace(tb.top(), tb.left(), err);
		err->append(tb.left());
		tb.left() = err;
	}
	else
	{
		UnfinishedOperator* unf = new UnfinishedOperator(new ErrorDescriptor(text_, ""));
		unf->addNext();
		if (tb.top()) tb.unfinished().last()->addNext(unf);
		else tb.top() = unf;

		tb.unfinished().append(unf);
	}
}
} /* namespace InteractionBase */
