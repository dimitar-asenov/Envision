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
 * HOOExpression.cpp
 *
 *  Created on: Feb 8, 2012
 *      Author: Dimitar Asenov
 */

#include "handlers/HOOExpression.h"

#include "OOModel/headers/allOOModelNodes.h"

#include <QtCore/QDebug>

namespace OOInteraction {

HOOExpression::HOOExpression()
{

}

HOOExpression* HOOExpression::instance()
{
	static HOOExpression h;
	return &h;
}

void HOOExpression::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto e = dynamic_cast<OOModel::Expression*>(target->node());
	qDebug() << toString(e);
	GenericHandler::keyPressEvent(target, event);
}


QString HOOExpression::toString(OOModel::Expression* expression)
{
	if (!expression)
	{
		return QString();
	}
	else if (auto e = dynamic_cast<OOModel::BinaryOperation*>(expression) )
	{
		QString result = toString(e->left());
		switch(e->op())
		{
			case OOModel::BinaryOperation::TIMES: result +="*"; break;
			case OOModel::BinaryOperation::DIVIDE: result +="/"; break;
			case OOModel::BinaryOperation::REMAINDER: result +="%"; break;
			case OOModel::BinaryOperation::PLUS: result +="+"; break;
			case OOModel::BinaryOperation::MINUS: result +="-"; break;
			case OOModel::BinaryOperation::LEFT_SHIFT: result +="<<"; break;
			case OOModel::BinaryOperation::RIGHT_SHIFT_SIGNED: result +=">>"; break;
			case OOModel::BinaryOperation::RIGHT_SHIFT_UNSIGNED: result +=">>>"; break;
			case OOModel::BinaryOperation::LESS: result +="<"; break;
			case OOModel::BinaryOperation::GREATER: result +=">"; break;
			case OOModel::BinaryOperation::LESS_EQUALS: result +="<="; break;
			case OOModel::BinaryOperation::GREATER_EQUALS: result +=">="; break;
			case OOModel::BinaryOperation::EQUALS: result +="="; break;
			case OOModel::BinaryOperation::NOT_EQUALS: result +="!="; break;
			case OOModel::BinaryOperation::XOR: result +="^"; break;
			case OOModel::BinaryOperation::AND: result +="&"; break;
			case OOModel::BinaryOperation::OR: result +="|"; break;
			case OOModel::BinaryOperation::CONDITIONAL_AND: result +="&&"; break;
			case OOModel::BinaryOperation::CONDITIONAL_OR: result +="||"; break;
			case OOModel::BinaryOperation::ARRAY_INDEX: result +="["; break;
			default: return QString();
		}
		result += toString(e->right());

		if (e->op() == OOModel::BinaryOperation::ARRAY_INDEX) result += "]";
		return result;
	}
	else if ( auto e = dynamic_cast<OOModel::VariableAccess*>(expression) )
	{
		QString prefix = toString(e->prefix());
		if (!prefix.isEmpty()) prefix += ".";
		return prefix + e->ref()->path().split(':').last();
	}
	else if ( auto e = dynamic_cast<OOModel::IntegerLiteral*>(expression) )
	{
		return QString::number( e->value() );
	}
	else if ( auto e = dynamic_cast<OOModel::UnfinishedOperator*>(expression) )
	{
		QString result;
		for (int i=0; i< e->operands()->size(); ++i)
		{
			result += e->delimiters()->at(i)->get();
			result += toString(e->operands()->at(i));
		}
		if (e->delimiters()->size() > e->operands()->size())
			result += e->delimiters()->last()->get();
		return result;
	}
	else if ( auto e = dynamic_cast<OOModel::ErrorExpression*>(expression) )
	{
		return e->prefix() + toString(e->arg()) + e->postfix();
	}
	else if ( auto e = dynamic_cast<OOModel::IntegerLiteral*>(expression) )
	{
		return QString::number( e->value() );
	}
	else if ( dynamic_cast<OOModel::EmptyExpression*>(expression) )
	{
		return "";
	}
	else return "NOT Implemented";
}

} /* namespace OOInteraction */
