/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * OOExpressionBuilder.h
 *
 *  Created on: Jan 12, 2012
 *      Author: Dimitar Asenov
 */

#ifndef OOInteraction_OOEXPRESSIONBUILDER_H_
#define OOInteraction_OOEXPRESSIONBUILDER_H_

#include "../oointeraction_api.h"

#include "InteractionBase/src/expression_editor/ExpressionVisitor.h"
#include "InteractionBase/src/expression_editor/Expression.h"

namespace Model {
	class Node;
}

namespace OOModel {
	class Expression;
}

namespace OOInteraction {

class OOINTERACTION_API OOExpressionBuilder : public Interaction::ExpressionVisitor {
	public:

		static OOModel::Expression* getOOExpression(const QString& exprText);
		OOModel::Expression* getOOExpression(Interaction::Expression* expression);

		virtual void visit(Interaction::Empty* empty);
		virtual void visit(Interaction::Value* val);
		virtual void visit(Interaction::Operator* op);
		virtual void visit(Interaction::UnfinishedOperator* unfinished);

	protected:
		OOModel::Expression* expression;

		void createErrorExpression(Interaction::Operator* op);
};

} /* namespace InteractionBase */
#endif /* OOInteraction_OOEXPRESSIONBUILDER_H_ */
