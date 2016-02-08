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

#include "Value.h"
#include "ExpressionVisitor.h"

namespace Interaction {

Value::Value(const QString& text, Operator* parent) : Expression{type(), parent}, text_{text}
{
}

QString Value::renderText()
{
	return text_;
}

void Value::accept(ExpressionVisitor* visitor)
{
	visitor->visit(this);
}

ExpressionContext Value::findContext(int cursor_pos)
{
	ExpressionContext c;

	if (cursor_pos > 0 && cursor_pos < text_.size())
	{
		c.setLeftType(ExpressionContext::InValue);
		c.setRightType(ExpressionContext::InValue);
		c.setLeft(this);
		c.setRight(this);
		c.setLeftText(text_.left(cursor_pos));
		c.setRightText(text_.mid(cursor_pos));
	} else if (cursor_pos == 0)
	{
		c.setRightType(ExpressionContext::Value);
		c.setRight(this);
		c.setRightText(text_);
	}
	else
	{
		c.setLeftType(ExpressionContext::Value);
		c.setLeft(this);
		c.setLeftText(text_);
	}

	return c;
}

}
