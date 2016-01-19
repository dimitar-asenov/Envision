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

#pragma once

#include "../oointeraction_api.h"

#include "OOModel/src/expressions/Expression.h"
#include "OOModel/src/expressions/UnaryOperation.h"
#include "OOModel/src/expressions/BinaryOperation.h"
#include "OOModel/src/expressions/AssignmentExpression.h"
#include "InteractionBase/src/expression_editor/OperatorDescriptor.h"

namespace OOInteraction {

class OOINTERACTION_API OOOperatorDescriptor : public Interaction::OperatorDescriptor {
	public:
		using CreateFunction = std::function<OOModel::Expression*(const QList<OOModel::Expression*>& operands)>;

		OOOperatorDescriptor(const QString& name, const QString& signature, int precedence,
				Associativity associativity);

		OOOperatorDescriptor(const QString& name, const QString& signature, int precedence,
						Associativity associativity, CreateFunction createFunction);

		virtual OOModel::Expression* create(const QList<OOModel::Expression*>& operands);

		// Useful creationFunctions
		template<OOModel::UnaryOperation::OperatorTypes op>
		static OOModel::Expression* unary(const QList<OOModel::Expression*>& operands);

		template<OOModel::BinaryOperation::OperatorTypes op>
		static OOModel::Expression* binary(const QList<OOModel::Expression*>& operands);

		template<OOModel::AssignmentExpression::AssignmentTypes op>
		static OOModel::Expression* assignment(const QList<OOModel::Expression*>& operands);

	private:
		CreateFunction createFunction_{};
};

template<OOModel::UnaryOperation::OperatorTypes op>
OOModel::Expression* OOOperatorDescriptor::unary(const QList<OOModel::Expression*>& operands)
{
	auto opr = new OOModel::UnaryOperation{};
	opr->setOp(op);
	opr->setOperand(operands.first());
	return opr;
}

template<OOModel::BinaryOperation::OperatorTypes op>
OOModel::Expression* OOOperatorDescriptor::binary(const QList<OOModel::Expression*>& operands)
{
	auto opr = new OOModel::BinaryOperation{};
	opr->setOp(op);
	opr->setLeft(operands.first());
	opr->setRight(operands.last());
	return opr;
}

template<OOModel::AssignmentExpression::AssignmentTypes op>
OOModel::Expression* OOOperatorDescriptor::assignment(const QList<OOModel::Expression*>& operands)
{
	auto opr = new OOModel::AssignmentExpression{};
	opr->setOp(op);
	opr->setLeft(operands.first());
	opr->setRight(operands.last());
	return opr;
}

}
