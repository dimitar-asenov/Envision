/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "ExpressionContext.h"

#include "Expression.h"
#include "Operator.h"
#include "Value.h"

namespace Interaction {

ExpressionContext::ExpressionContext()
	:	left_type_{None}, right_type_{None}, left_{nullptr}, right_{nullptr}, left_text_{QString::Null{}},
	  right_text_{QString::Null{}}, left_delim_{-1}, right_delim_{-1}
{
}

Operator* ExpressionContext::leftOp()
{
	return dynamic_cast<Operator*> (left_);
}

Operator* ExpressionContext::rightOp()
{
	return dynamic_cast<Operator*> (right_);
}

Value* ExpressionContext::leftValue()
{
	return dynamic_cast<Interaction::Value*> (left_);
}
Value* ExpressionContext::rightValue()
{
	return dynamic_cast<Interaction::Value*> (right_);
}

Expression* ExpressionContext::leftExp()
{
	return left_;
}

Expression* ExpressionContext::rightExp()
{
	return right_;
}

bool ExpressionContext::isLeftDelimPostfix()
{
	return left_delim_ == leftOp()->size();
}

bool ExpressionContext::isRightDelimPostfix()
{
	return right_delim_ == rightOp()->size();
}

}
