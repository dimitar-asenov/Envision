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
 * ExpressionContext.h
 *
 *  Created on: Jan 10, 2012
 *      Author: Dimitar Asenov
 */

#ifndef INTERACTIONBASE_EXPRESSIONCONTEXT_H_
#define INTERACTIONBASE_EXPRESSIONCONTEXT_H_

#include "../interactionbase_api.h"

namespace Interaction {

class Expression;
class Operator;
class Value;

class INTERACTIONBASE_API ExpressionContext {
	public:

		enum TokenType {InValue, InOpDelim, Value, OpDelim, OpBoundary, Expr, None, Empty};
		ExpressionContext();

		ExpressionContext& setLeftType(TokenType type);
		ExpressionContext& setRightType(TokenType type);

		ExpressionContext& setLeft(Expression* expr);
		ExpressionContext& setRight(Expression* expr);

		ExpressionContext& setLeftText(QString text);
		ExpressionContext& setRightText(QString text);

		ExpressionContext& setLeftDelim(int delim);
		ExpressionContext& setRightDelim(int delim);

		TokenType leftType();
		TokenType rightType();

		Operator* leftOp();
		Operator* rightOp();

		Interaction::Value* leftValue();
		Interaction::Value* rightValue();

		Expression* leftExp();
		Expression* rightExp();

		int leftDelim();
		int rightDelim();

		bool isLeftDelimPrefix();
		bool isRightDelimPrefix();

		bool isLeftDelimPostfix();
		bool isRightDelimPostfix();

		bool isLeftDelimInfix();
		bool isRightDelimInfix();

		QString leftText();
		QString rightText();

	private:
		TokenType left_type_;
		TokenType right_type_;

		Expression* left_;
		Expression* right_;

		QString left_text_;
		QString right_text_;

		int left_delim_;
		int right_delim_;
};

inline ExpressionContext& ExpressionContext::setLeftType(TokenType type) {left_type_ = type; return *this;}
inline ExpressionContext& ExpressionContext::setRightType(TokenType type) {right_type_ = type; return *this;}

inline ExpressionContext& ExpressionContext::setLeft(Expression* expr) {left_ = expr; return *this;}
inline ExpressionContext& ExpressionContext::setRight(Expression* expr) {right_ = expr; return *this;}

inline ExpressionContext& ExpressionContext::setLeftText(QString text) {left_text_ = text; return *this;}
inline ExpressionContext& ExpressionContext::setRightText(QString text) {right_text_ = text; return *this;}

inline ExpressionContext& ExpressionContext::setLeftDelim(int delim) {left_delim_ = delim; return *this;}
inline ExpressionContext& ExpressionContext::setRightDelim(int delim) {right_delim_ = delim; return *this;}

inline ExpressionContext::TokenType ExpressionContext::leftType() {return left_type_;}
inline ExpressionContext::TokenType ExpressionContext::rightType() {return right_type_;}

inline int ExpressionContext::leftDelim() {return left_delim_;}
inline int ExpressionContext::rightDelim() {return right_delim_;}

inline bool ExpressionContext::isLeftDelimPrefix() {return left_delim_ == 0; }
inline bool ExpressionContext::isRightDelimPrefix() { return right_delim_ == 0; }

inline bool ExpressionContext::isLeftDelimInfix() { return !isLeftDelimPrefix() && !isLeftDelimPostfix(); }
inline bool ExpressionContext::isRightDelimInfix() { return !isRightDelimPrefix() && !isRightDelimPostfix(); }

inline QString ExpressionContext::leftText() { return left_text_; }
inline QString ExpressionContext::rightText() { return right_text_; }

} /* namespace InteractionBase */
#endif /* INTERACTIONBASE_EXPRESSIONCONTEXT_H_ */
