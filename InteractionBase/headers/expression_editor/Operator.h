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
 * Operator.h
 *
 *  Created on: Jan 10, 2012
 *      Author: Dimitar Asenov
 */

#ifndef OPERATOR_H_
#define OPERATOR_H_

#include "interactionbase_api.h"

#include "Expression.h"

namespace InteractionBase {

class OperatorDescriptor;

class INTERACTIONBASE_API Operator : public Expression {
	public:
		Operator(OperatorDescriptor* descriptor, Operator* parent = nullptr);
		virtual ~Operator();

		virtual QString renderText();

		virtual ExpressionContext findContext(int cursor_pos);

		static int type();

		int size();
		const QList<Expression*> allOperands();

		Expression* first(bool remove_from_list = false);
		Expression* last(bool remove_from_list = false);
		Expression* at(int i, bool remove_from_list = false);
		bool contains(Expression* e);
		void clear();
		void remove(Expression* e);

		void append(Expression* e);
		void prepend(Expression* e);
		void insert(Expression* e, int pos);
		Expression* replace(Expression* oldExpr, Expression* newExpr);

		OperatorDescriptor* descriptor();

		virtual Expression* smallestRightmostSubExpr();
		virtual Expression* smallestLeftmostSubExpr();

		void globalExpressionBoundaries(Expression* e, int& begin, int& end);
		void globalDelimiterBoundaries(int delim, int& begin, int& end);

		virtual Expression* findCutExpression(bool leftside, QString cut_string);

	private:
		OperatorDescriptor* descriptor_;
		QList<Expression*> operands_;

		Expression* possiblyRemove(Expression* e, bool remove);
};

inline int Operator::type() { return 2; }
inline int Operator::size() { return operands_.size(); }
inline const QList<Expression*> Operator::allOperands() {return operands_;}
inline bool Operator::contains(Expression* e) {return operands_.contains(e);}
inline void Operator::append(Expression* e) { insert(e, operands_.size()); }
inline void Operator::prepend(Expression* e) { insert(e, 0); }
inline OperatorDescriptor* Operator::descriptor() { return descriptor_; }


} /* namespace InteractionBase */
#endif /* OPERATOR_H_ */
