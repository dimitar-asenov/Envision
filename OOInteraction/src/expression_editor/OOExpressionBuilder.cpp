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

#include "OOExpressionBuilder.h"

#include "OOOperatorDescriptorList.h"
#include "OOOperatorDescriptor.h"
#include "../OOInteractionException.h"

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
	expression = new OOModel::EmptyExpression{};
}

void OOExpressionBuilder::visit(Interaction::Value* val)
{
	if (val->text().isEmpty()) throw OOInteractionException("Trying to create an expression from an empty Value");

	if (val->text().startsWith('"'))
		  expression = new OOModel::StringLiteral{val->text().mid(1, val->text().size()-2)};
	else 	if (val->text().startsWith('\''))
		expression = new OOModel::CharacterLiteral{val->text().mid(1, val->text().size()-2)};
	else if (val->text().at(0).isDigit() &&
				(val->text().contains('.') || val->text().contains('E') || val->text().contains('e')))
		expression = new OOModel::FloatLiteral{val->text()};
	else if (val->text().at(0).isDigit() )
		 expression = new OOModel::IntegerLiteral{val->text()};
	else
		expression = new OOModel::ReferenceExpression{val->text()};
}

QChar OOExpressionBuilder::toChar(QString charLiteral, bool& ok)
{
	ok = false;
	if (charLiteral.length() < 3 || !charLiteral.startsWith('\'') || !charLiteral.endsWith('\'')) return {};

	if (charLiteral.length() == 3)
	{
		ok = true;
		return charLiteral.at(1);
	}

	if (charLiteral.at(1) != '\\') return {};


	if (charLiteral.length() == 4)
	{
		//Assume ok
		ok = true;
		if (charLiteral.at(2) == '\'') return '\'';
		if (charLiteral.at(2) == '\"') return '\"';
		if (charLiteral.at(2) == '\?') return '\?';
		if (charLiteral.at(2) == '\\') return '\\';
		if (charLiteral.at(2) == 'a')  return '\a';
		if (charLiteral.at(2) == 'b')  return '\b';
		if (charLiteral.at(2) == 'f')  return '\f';
		if (charLiteral.at(2) == 'n')  return '\n';
		if (charLiteral.at(2) == 'r')  return '\r';
		if (charLiteral.at(2) == 't')  return '\t';
		if (charLiteral.at(2) == 'v')  return '\v';
		if (charLiteral.at(2) == '0')  return '\0';
		ok = false;
	}

	Q_ASSERT(charLiteral.length() >= 4);
	if ((charLiteral.at(2) == 'x' || charLiteral.at(2) == 'u' || charLiteral.at(2) == 'U') && charLiteral.length() > 4)
	{
		return charLiteral.mid(3, charLiteral.length() - 1 - 3).toInt(&ok, 16);
	}

	// Finally try octal
	return charLiteral.mid(2, charLiteral.length() - 1 - 2).toInt(&ok, 8);
}

void OOExpressionBuilder::visit(Interaction::Operator* op)
{
	if (dynamic_cast<Interaction::ErrorDescriptor*>(op->descriptor()))
		createErrorExpression(op);
	else
	{
		QList<OOModel::Expression*> operands;
		for (auto e : op->operands())
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
		OOModel::UnfinishedOperator* unf = new OOModel::UnfinishedOperator{};
		int operand_index = 0;
		for (int i = 0; i <unfinished->numComplete(); ++i)
		{
			QString current = unfinished->descriptor()->signature().at(i);

			if (!Interaction::OperatorDescriptor::isDelimiter(current))
			{
				// If there are two expressions next to each other without an operator in between, put a space.
				if (lastDelimiter.isEmpty() && operand_index > 0) lastDelimiter = ' ';

				unf->delimiters()->append(new Model::Text{lastDelimiter});
				lastDelimiter.clear();
				unfinished->at(operand_index)->accept(this);
				++operand_index;
				unf->operands()->append(expression);
			}
			else
				lastDelimiter += current;
		}
		unf->delimiters()->append(new Model::Text{lastDelimiter}); // append the postfix, even if empty
		expression = unf;
	}
}

void OOExpressionBuilder::createErrorExpression(Interaction::Operator* op)
{
	OOModel::ErrorExpression* error = new OOModel::ErrorExpression{};
	auto desc = dynamic_cast<Interaction::ErrorDescriptor*>(op->descriptor());
	error->setPrefix(desc->errorPrefix());
	error->setPostfix(desc->errorPostfix());

	op->at(0)->accept(this);
	error->setArg(expression);

	expression = error;
}

}
