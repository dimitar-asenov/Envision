/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "expression_editor/OOExpressionBuilder.h"

#include "expression_editor/OOOperatorDescriptorList.h"
#include "expression_editor/OOOperatorDescriptor.h"
#include "OOInteractionException.h"

#include "OOModel/src/allOOModelNodes.h"
#include "InteractionBase/src/expression_editor/Value.h"
#include "InteractionBase/src/expression_editor/Operator.h"
#include "InteractionBase/src/expression_editor/UnfinishedOperator.h"
#include "InteractionBase/src/expression_editor/OperatorDescriptor.h"
#include "InteractionBase/src/expression_editor/ErrorDescriptor.h"
#include "InteractionBase/src/expression_editor/ExpressionEditor.h"

namespace OOInteraction {

OOModel::Expression* OOExpressionBuilder::getOOExpression(const QString& exprText)
{
	static Interaction::ExpressionEditor editor;
	editor.setOperatorDescriptors(OOOperatorDescriptorList::instance());
	OOExpressionBuilder builder;
	return builder.getOOExpression( editor.parse(exprText) );
}

OOModel::Expression* OOExpressionBuilder::getOOExpression(Interaction::Expression* expression)
{
	expression->accept(this);
	OOModel::Expression* e = this->expression;
	this->expression = nullptr;
	return e;
}

void OOExpressionBuilder::visit(Interaction::Empty*)
{
	expression = new OOModel::EmptyExpression();
}

void OOExpressionBuilder::visit(Interaction::Value* val)
{
	if (val->text().isEmpty()) throw OOInteractionException("Trying to create an expression from an empty Value");

	bool isInt = false;
	int value = val->text().toInt(&isInt);

	// TODO: this should be done in a more extensible way
	if (isInt)
		expression = new OOModel::IntegerLiteral(value);
	else if (val->text() == "true" || val->text() == "false")
		expression = new OOModel::BooleanLiteral( val->text() == "true" );
	else if (val->text() == "null")
		expression = new OOModel::NullLiteral();
	else if (val->text() == "this")
		expression = new OOModel::ThisExpression();
	else if (val->text() == "auto")
		expression = new OOModel::AutoTypeExpression();
	else if (val->text() == "int")
		expression = new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT);
	else if (val->text() == "long")
		expression = new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
	else if (val->text() == "uint")
		expression = new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT);
	else if (val->text() == "ulong")
		expression = new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
	else if (val->text() == "float")
		expression = new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::FLOAT);
	else if (val->text() == "double")
		expression = new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::DOUBLE);
	else if (val->text() == "bool")
		expression = new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN);
	else if (val->text() == "char")
		expression = new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
	else if (val->text() == "void")
		expression = new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::VOID);
	else if (val->text().startsWith('"'))
		expression = new OOModel::StringLiteral(val->text().mid(1, val->text().size()-2));
	else
		expression = new OOModel::ReferenceExpression(val->text());
}

void OOExpressionBuilder::visit(Interaction::Operator* op)
{
	if (dynamic_cast<Interaction::ErrorDescriptor*>(op->descriptor()))
		createErrorExpression(op);
	else
	{
		QList<OOModel::Expression*> operands;
		for(auto e : op->operands())
		{
			e->accept(this);
			operands.append(expression);
		}
		OOOperatorDescriptor* desc = static_cast<OOOperatorDescriptor*>(op->descriptor());
		expression = desc->create(operands);
	}
}

void OOExpressionBuilder::visit(Interaction::UnfinishedOperator* unfinished)
{
	if (dynamic_cast<Interaction::ErrorDescriptor*>(unfinished->descriptor()))
			createErrorExpression(unfinished);
	else
	{
		QString lastDelimiter;
		OOModel::UnfinishedOperator* unf = new OOModel::UnfinishedOperator();
		int operand_index = 0;
		for(int i = 0; i <unfinished->numComplete(); ++i)
		{
			QString current = unfinished->descriptor()->signature().at(i);
			bool current_is_expr = current == "id" || current == "expr";

			if (current_is_expr)
			{
				unf->delimiters()->append(new Model::Text(lastDelimiter));
				lastDelimiter.clear();
				unfinished->at(operand_index)->accept(this);
				++operand_index;
				unf->operands()->append(expression);
			}
			else
				lastDelimiter += current;
		}
		unf->delimiters()->append(new Model::Text(lastDelimiter)); // append the postfix, even if empty
		expression = unf;
	}
}

void OOExpressionBuilder::createErrorExpression(Interaction::Operator* op)
{
	OOModel::ErrorExpression* error = new OOModel::ErrorExpression();
	auto desc = dynamic_cast<Interaction::ErrorDescriptor*>(op->descriptor());
	error->setPrefix(desc->errorPrefix());
	error->setPostfix(desc->errorPostfix());

	op->at(0)->accept(this);
	error->setArg(expression);

	expression = error;
}

} /* namespace InteractionBase */
