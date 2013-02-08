/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#ifndef INTERACTIONBASE_EXPRESSION_H_
#define INTERACTIONBASE_EXPRESSION_H_

#include "../interactionbase_api.h"

#include "ExpressionContext.h"

namespace Interaction {

class Operator;
class ExpressionVisitor;

class INTERACTIONBASE_API Expression {
	public:
		Expression(const int type, Operator* parent = nullptr);
		virtual ~Expression();

		virtual QString renderText() = 0;
		virtual void accept(ExpressionVisitor* visitor) = 0;

		int length() {return renderText().size();}
		virtual ExpressionContext findContext(int cursor_pos) = 0;

		Operator* parent() {return parent_;}
		void setParent(Operator* new_parent);

		int type() {return type_;}

		// Returns this by default
		virtual Expression* smallestRightmostSubExpr();
		// Returns this by default
		virtual Expression* smallestLeftmostSubExpr();

		// Return nullptr by default, meaning can not cut this expression. This method is used when growing an operator.
		virtual Expression* findCutExpression(bool leftside, QString cut_string);

		void removeFromParent();
	private:
		const int type_;
		Operator* parent_;
};

} /* namespace InteractionBase */
#endif /* INTERACTIONBASE_EXPRESSION_H_ */
