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

#include "ExpressionTreeUtils.h"

#include "Value.h"
#include "Empty.h"
#include "Operator.h"
#include "UnfinishedOperator.h"
#include "ErrorDescriptor.h"
#include "OperatorDescriptor.h"

namespace Interaction {

void ExpressionTreeUtils::fixTop(Expression*& top)
{
	fixPrecedence(top);
	fixWrongIds(top);
}

Expression* ExpressionTreeUtils::replace(Expression*& top, Expression* oldExpr, Expression* newExpr)
{
	if (oldExpr == top)
	{
		top = newExpr;
		newExpr->setParent(nullptr);
		return oldExpr;
	}
	else
		return oldExpr->parent()->replaceOperand(oldExpr, newExpr);
}

void ExpressionTreeUtils::rotateRight(Expression*& top, Operator* child, Operator* parent)
{
	parent->first(true);
	replace(top, parent, child);
	Expression* branch = child->last(true);
	child->append(parent);
	parent->prepend(branch);

}

void ExpressionTreeUtils::rotateLeft(Expression*& top, Operator* child, Operator* parent)
{
	parent->last(true);
	replace(top, parent, child);
	Expression* branch = child->first(true);
	child->prepend(parent);
	parent->append(branch);
}

void ExpressionTreeUtils::fixPrecedence(Expression*& top)
{
	bool more_iterations_needed = true;
	while (more_iterations_needed ) more_iterations_needed = fixExprPrecedence(top, top);
}

bool ExpressionTreeUtils::fixExprPrecedence(Expression*& top, Expression* e)
{
	if ( dynamic_cast<Value*> (e)) return false;
	if ( dynamic_cast<Empty*> (e)) return false;

	Operator* op = dynamic_cast<Operator*> (e);

	bool more_iterations_needed = true;
	while (more_iterations_needed)
	{
		more_iterations_needed = false;

		// Fix all children
		for (int operand = 0; operand < op->size(); ++operand)
			more_iterations_needed = fixExprPrecedence(top, op->at(operand)) || more_iterations_needed;
	}

	//Look left
	if (op->descriptor()->prefix().isEmpty())
	{
		Operator* left = dynamic_cast<Operator*> (op->first());
		if (left && left->descriptor()->postfix().isEmpty())
		{
			if (op->descriptor()->precedence() < left->descriptor()->precedence() // Must rotate because of precedence

				 // Must rotate because of associativity. This assumes that the associativity of different operators at
				 // the same precedence level is the same.
				 || ( (op->descriptor()->precedence() == left->descriptor()->precedence())
						&& op->descriptor()->associativity() == OperatorDescriptor::RightAssociative)
				 )
			{
				rotateRight(top, left, op);
				return true;
			}
		}
	}

	//Look right
	if (op->descriptor()->postfix().isEmpty())
	{
		Operator* right = dynamic_cast<Operator*> (op->last());
		if (right && right->descriptor()->prefix().isEmpty())
		{
			if (op->descriptor()->precedence() < right->descriptor()->precedence() // Must rotate because of precedence

				 // Must rotate because of associativity. This assumes that the associativity of different operators at
				 // the same precedence level is the same.
				 || ( (op->descriptor()->precedence() == right->descriptor()->precedence())
						&& op->descriptor()->associativity() == OperatorDescriptor::LeftAssociative)
				 )
			{
				rotateLeft(top, right, op);
				return true;
			}
		}
	}

	return false;
}

void ExpressionTreeUtils::grow(Expression*& top, Operator* op, bool leftside)
{
	bool rightside = !leftside;

	// Can't grow if there is no parent
	if ( !op->parent() )
		return;


	// Can't grow from a side where there is no delimiter
	if ( (leftside && op->descriptor()->prefix().isEmpty() ) || (rightside && op->descriptor()->postfix().isEmpty()) )
		return;

	Operator* parent = op->parent();

	// Get context
	int delim_begin;
	int delim_end;
	op->globalDelimiterBoundaries(leftside ? 0 : op->descriptor()->numOperands(), delim_begin, delim_end);
	ExpressionContext c = top->findContext(leftside ? delim_begin: delim_end);

	// If this is an expression in the middle of two delimiters that belong to the same operator
	// this typically means that it can not be grown any more from only a single side...
	bool wrap_parent = false;
	if (   ( leftside && (c.leftType() == ExpressionContext::None || c.leftType() == ExpressionContext::OpBoundary) )
		 || ( rightside && (c.rightType() == ExpressionContext::None || c.rightType() == ExpressionContext::OpBoundary) )
			 )
	{
		// .. EXCEPT: when both delimiters are the same as the delimiter we're trying to grow and the
		// direction opposite of the growth direction is an end delimiter. In that case we simply
		// engulf the entire parent.
		op->globalDelimiterBoundaries(leftside ? op->descriptor()->numOperands() : 0, delim_begin, delim_end);
		ExpressionContext c_other = top->findContext(leftside ? delim_end : delim_begin );
		if (   ( leftside && c_other.rightType() == ExpressionContext::OpBoundary
					&& c_other.rightText() == op->descriptor()->postfix().join("")
					&& c_other.rightDelim() == c_other.rightOp()->size())
			 || ( rightside && c_other.leftType() == ExpressionContext::OpBoundary
					&& c_other.leftText() == op->descriptor()->prefix().join("") && c_other.leftDelim() == 0) )
			wrap_parent = true;
		else
		return;
	}

	// Special case when the parent ends with a pre/postfix in the direction we're growing.
	// In that case we must wrap the whole operator as we can not break the delimiters.
	wrap_parent = wrap_parent || ( !(leftside && parent->descriptor()->prefix().isEmpty())
											 && !(rightside && parent->descriptor()->postfix().isEmpty()));
	if (wrap_parent)
	{
		Expression* placeholder = new Empty{};
		replace(top, parent, placeholder);
		replace(top, op, leftside ? op->first(true) : op->last(true));
		if (leftside) op->prepend(parent);
		else op->append(parent);
		delete replace(top, placeholder, op);

		return;
	}

	// Find the expression that must be wrapped
	Operator* top_op = parent;
	Operator* child = op;
	while ( (leftside ? top_op->last() : top_op->first()) != child)
	{
		child = top_op;
		top_op = top_op->parent();
		if (!top_op) return;
	}
	Expression* to_wrap = leftside ? top_op->first()->smallestRightmostSubExpr()
											 : top_op->last()->smallestLeftmostSubExpr();

	// Do the exchange --------------------------------------

	// Disconnect top_op from the the entire tree and from it's first and last
	// Note that if we've reached this point then first and last must be two different nodes
	Expression* top_op_placeholder = new Empty{};
	replace(top, top_op, top_op_placeholder);
	Expression* top_op_last = top_op->last(true);   // Special case when rightside: top_op_last == to_wrap
	Expression* top_op_first = top_op->first(true); // Special case when leftside: top_op_first == to_wrap

	// Disconnect the to_wrap expression if not yet disconnected
	Expression* to_wrap_placeholder = nullptr;
	if ( (leftside && to_wrap != top_op_first) || (rightside && to_wrap != top_op_last) )
	{
		to_wrap_placeholder = new Empty{};
		replace(top, to_wrap, to_wrap_placeholder);
	}

	// Disconnect the old_wrapped expression
	Expression* old_wrap = leftside ? op->first(true) : op->last(true); // This is the content that was previously wrapped
	// Disconnect the left and right children of top_op.

	// Make the necessary connections
	if (leftside)
	{
		op->prepend(top_op);
		top_op->prepend(to_wrap);
		top_op->append(old_wrap);
		//Consider the special case
		if (top_op_first == to_wrap)
		{
			delete replace(top, top_op_placeholder, top_op_last );
		}
		else
		{
			delete replace(top, top_op_placeholder, top_op_first );
			delete replace(top, to_wrap_placeholder, top_op_last);
		}
	}
	else
	{
		op->append(top_op);
		top_op->prepend(old_wrap);
		top_op->append(to_wrap);
		//Consider the special case
		if (top_op_last == to_wrap)
		{
			delete replace(top, top_op_placeholder, top_op_first );
		}
		else
		{
			delete replace(top, top_op_placeholder, top_op_last );
			delete replace(top, to_wrap_placeholder, top_op_first);
		}
	}

	fixTop(top);
}

void ExpressionTreeUtils::shrink(Expression*& top, Operator* op, bool leftside)
{
	// Can't shrink if only an atomic value left
	if ( ( leftside ? op->first()->type() : op->last()->type() ) != Operator::type() )
		return;

	// Can't shrink from a side where there is no delimiter
	if ( (leftside && op->descriptor()->prefix().isEmpty() ) || (!leftside && op->descriptor()->postfix().isEmpty()))
		return;

	Expression* new_border_expr = (leftside ? op->first() : op->last())
			->findCutExpression(leftside, leftside ?
					op->descriptor()->postfix().join("") : op->descriptor()->prefix().join(""));
	if (new_border_expr == nullptr) return;

	Operator* cut_op = new_border_expr->parent();
	Operator* cut_op_parent = cut_op->parent();

	// Rewire the tree
	Expression* op_placeholder = new Empty;
	replace(top, op, op_placeholder);

	if (leftside)
	{
		cut_op->last(true);
		cut_op_parent->first(true);
		cut_op_parent->prepend(new_border_expr);
		cut_op->append(op);
	}
	else
	{
		cut_op->first(true);
		cut_op_parent->last(true);
		cut_op_parent->append(new_border_expr);
		cut_op->prepend(op);
	}

	delete replace(top, op_placeholder, cut_op);

	fixTop(top);
}

void ExpressionTreeUtils::fixWrongIds(Expression*& top)
{
	QList<Expression*> toFix;
	toFix << top;

	while (!toFix.isEmpty())
	{
		auto e = toFix.first();
		toFix.removeFirst();

		if (auto op = dynamic_cast<Operator*>(e))
		{
			bool isUnfinished = dynamic_cast<UnfinishedOperator*>(op);
			if (!isUnfinished)
			{
				bool isError = dynamic_cast<ErrorDescriptor*>(op->descriptor());
				if (!isError)
				{
					bool badId = false;
					int operandIndex = 0;
					for (auto s : op->descriptor()->signature())
					{
						if (s == "id")
						{
							auto v = dynamic_cast<Value*> (op->operands().at(operandIndex));
							badId = badId || !v || v->text().isEmpty() || !(v->text()[0].isLetter() || v->text()[0] == '_');
							if (badId) break;
						}

						if (!OperatorDescriptor::isDelimiter(s)) ++operandIndex;
					}

					if (badId) op = UnfinishedOperator::replaceFinishedWithUnfinished(top, op);
				}
			}

			for (auto operand : op->operands())
				toFix.append(operand);
		}
	}
}

}
