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

#include "Operator.h"
#include "../InteractionBaseException.h"

#include "OperatorDescriptor.h"
#include "ExpressionVisitor.h"

namespace Interaction {

Operator::Operator(OperatorDescriptor* descriptor, Operator* parent)
	: Expression{type(), parent}, descriptor_{descriptor}, do_not_delete_transient_descriptor_{false}
{
}

void Operator::accept(ExpressionVisitor* visitor)
{
	visitor->visit(this);
}

Operator::~Operator()
{
	clear();
	if (descriptor()->isTransient() && !do_not_delete_transient_descriptor_) delete descriptor();
}

Expression* Operator::possiblyRemove(Expression* e, bool remove)
{
	if (remove && e)
	{
		operands_.removeAll(e);
		e->setParent(nullptr);
	}

	return e;
}

Expression* Operator::first(bool remove_from_list)
{
	Expression* tmp = operands_.isEmpty()?nullptr:operands_.first();
	return possiblyRemove(tmp, remove_from_list);
}

Expression* Operator::last(bool remove_from_list)
{
	Expression* tmp = operands_.isEmpty()?nullptr:operands_.last();
	return possiblyRemove(tmp, remove_from_list);
}

Expression* Operator::at(int i, bool remove_from_list)
{
	Expression* tmp = operands_.size() <= i ?nullptr:operands_.at(i);
	return possiblyRemove(tmp, remove_from_list);
}

void Operator::clear()
{
	for (int i = 0; i<operands_.size(); ++i)
		delete operands_.at(i);
	operands_.clear();
}

void Operator::remove(Expression* e)
{
	operands_.removeAll(e);
	if (e) e->setParent(nullptr);
}

void Operator::insert(Expression* e, int pos)
{
	if (e)
	{
		e->setParent(this);
		operands_.insert(pos, e);
	}
}

Expression* Operator::replaceOperand(Expression* oldExpr, Expression* newExpr)
{
	int i = operands_.indexOf(oldExpr);
	if (i <0) throw InteractionBaseException{"Invalid replacement operation. Could not find expression to replace."};

	newExpr->setParent(this);
	operands_.replace(i, newExpr);
	return oldExpr;
}

QString Operator::renderText()
{
	QStringList children;
	children.append(descriptor_->prefix().join(""));
	children.append(operands_.first()->renderText());

	for (int i=0; i < descriptor_->infixes().size(); ++i)
	{
		children.append(descriptor_->infix(i).join(""));
		children.append(operands_.at(i+1)->renderText());
	}

	children.append(descriptor_->postfix().join(""));
	return children.join("");
}

ExpressionContext Operator::findContext(int cursor_pos)
{
	// Find which child contains the cursor

	// If the cursor is at the beginning
	if (cursor_pos == 0)
	{
		if (descriptor_->prefix().isEmpty()) return first()->findContext(0);
		else
		{
			ExpressionContext c;
			c.setRight(this);
			c.setRightDelim(0);
			c.setRightText(descriptor_->prefix().join(""));
			c.setRightType(ExpressionContext::Expr);
			return c;
		}
	}

	QStringList delims = descriptor_->delimiters();

	// If the cursor is at the end
	if (cursor_pos == length())
	{
		if (descriptor_->postfix().isEmpty()) return last()->findContext(last()->length());
		else
		{
			ExpressionContext c;
			c.setLeft(this);
			c.setLeftDelim(delims.size()-1);
			c.setLeftText(descriptor_->postfix().join(""));
			c.setLeftType(ExpressionContext::Expr);
			return c;
		}
	}

	// The cursor must be somewhere in the middle
	int length_so_far = 0;
	for (int d = 0; d < delims.size(); ++d)
	{
		QString current_d = delims.at(d);
		length_so_far += current_d.size();

		// Inside a delimiter
		if (cursor_pos < length_so_far)
		{
			ExpressionContext c;
			c.setLeft(this);
			c.setRight(this);
			c.setLeftDelim(d);
			c.setRightDelim(d);
			c.setLeftType(ExpressionContext::InOpDelim);
			c.setRightType(ExpressionContext::InOpDelim);

			int cursor_inside = cursor_pos - (length_so_far - current_d.size());
			c.setLeftText(current_d.left(cursor_inside));
			c.setRightText(current_d.mid(cursor_inside));
			return c;
		}

		// Just after a delimiter. Note: this can not be the last delimiter, this is handled above.
		if (cursor_pos == length_so_far)
		{
			ExpressionContext c = operands_.at(d)->findContext(0);
			c.setLeft(this);
			c.setLeftDelim(d);
			c.setLeftText(delims.at(d));

			// If d is the last 'visible' delimiter this is an OP, otherwise it is a boundary
			c.setLeftType( (d == delims.size()-2) && delims.last().isEmpty() ? ExpressionContext::OpDelim
																								  : ExpressionContext::OpBoundary);
			return c;
		}

		// Inside a child operand
		int child_length = operands_.at(d)->length();
		length_so_far += child_length;
		if (cursor_pos < length_so_far)
		{
			int cursor_inside = cursor_pos - (length_so_far - child_length);
			return operands_.at(d)->findContext(cursor_inside);
		}

		// Just after a child operand
		if (cursor_pos == length_so_far)
		{
			ExpressionContext c = operands_.at(d)->findContext(child_length);
			c.setRight(this);
			c.setRightDelim(d+1);
			c.setRightText(delims.at(d+1));

			// If (d+1) is the first 'visible' delimiter this is an OP, otherwise it is a boundary
			c.setRightType( (d == 0) && delims.first().isEmpty() ? ExpressionContext::OpDelim : ExpressionContext::OpBoundary);
			return c;
		}
	}

	// This end should not be reached
	throw InteractionBaseException{"No context found"};
}

Expression* Operator::smallestRightmostSubExpr()
{
	if (descriptor_->postfix().isEmpty()) return last()->smallestRightmostSubExpr();
	else return this;
}

Expression* Operator::smallestLeftmostSubExpr()
{
	if (descriptor_->prefix().isEmpty()) return first()->smallestLeftmostSubExpr();
	else return this;
}

void Operator::globalExpressionBoundaries(Expression* e, int& begin, int& end)
{
	if (parent()) parent()->globalExpressionBoundaries(this, begin, end);
	else begin = 0;

	QStringList delims = descriptor_->delimiters();
	for (int i = 0; i<size(); ++i)
	{
		begin += delims.at(i).size();
		end = begin + operands_.at(i)->length();
		if (operands_.at(i) == e)
			return;
		begin = end;
	}
}

void Operator::globalDelimiterBoundaries(int delim, int& begin, int& end)
{
	if (parent()) parent()->globalExpressionBoundaries(this, begin, end);
	else begin = 0;

	end = begin + descriptor_->prefix().join("").size();

	QStringList delims = descriptor_->delimiters();
	for (int i = 1; i<=delim; ++i)
	{
		begin = end + operands_.at(i-1)->length();
		end = begin + delims.at(i).size();
	}
}

Expression* Operator::findCutExpression(bool leftside, QString cut_string)
{
	Expression* result = nullptr;

	// If there is no delimiter, try to cut the subexpression closer to the cutting side
	if ( leftside ? descriptor_->prefix().isEmpty() : descriptor_->postfix().isEmpty() )
		result = leftside ? first()->findCutExpression(leftside, cut_string)
								: last()->findCutExpression(leftside, cut_string);

	if (result == nullptr)
	{
		// Try to cut this operator directly.
		if (leftside)
			result = descriptor_->postfix().isEmpty() || descriptor_->postfix().join("") == cut_string ? last() : nullptr;
		else
			result = descriptor_->prefix().isEmpty() || descriptor_->prefix().join("") == cut_string  ? first() : nullptr;
	}

	return result;
}

}
