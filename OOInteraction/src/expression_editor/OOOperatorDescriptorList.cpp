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
 * OOOperatorDescriptorList.cpp
 *
 *  Created on: Jan 18, 2012
 *      Author: Dimitar Asenov
 */

#include "expression_editor/OOOperatorDescriptorList.h"

#include "expression_editor/operators/UnaryOperatorDescriptor.h"
#include "expression_editor/operators/BinaryOperatorDescriptor.h"
#include "expression_editor/operators/CastDescriptor.h"
#include "expression_editor/operators/CommaDescriptor.h"
#include "expression_editor/operators/NewArrayDescriptor.h"
#include "expression_editor/operators/InitializerDescriptor.h"
#include "expression_editor/operators/MemberOperatorDescriptor.h"
#include "expression_editor/operators/CallDescriptor.h"
#include "expression_editor/operators/ArrayTypeDescriptor.h"

namespace OOInteraction {

OOOperatorDescriptorList* OOOperatorDescriptorList::instance()
{
	static OOOperatorDescriptorList theInstance;
	return &theInstance;
}

void OOOperatorDescriptorList::initializeWithDefaultOperators()
{
	// Unary operators
	instance()->addDescriptor(new UnaryOperatorDescriptor(OOModel::UnaryOperation::PREINCREMENT,
			"preincrement", "++ expr", 1, 2, Interaction::OperatorDescriptor::RightAssociative));
	instance()->addDescriptor(new UnaryOperatorDescriptor(OOModel::UnaryOperation::PREDECREMENT,
			"predecrement", "-- expr", 1, 2, Interaction::OperatorDescriptor::RightAssociative));
	instance()->addDescriptor(new UnaryOperatorDescriptor(OOModel::UnaryOperation::POSTINCREMENT,
			"postincrement", "expr ++", 1, 2, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new UnaryOperatorDescriptor(OOModel::UnaryOperation::POSTDECREMENT,
			"postdecrement", "expr --", 1, 2, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new UnaryOperatorDescriptor(OOModel::UnaryOperation::PLUS,
			"unary plus", "+ expr", 1, 2, Interaction::OperatorDescriptor::RightAssociative));
	instance()->addDescriptor(new UnaryOperatorDescriptor(OOModel::UnaryOperation::MINUS,
			"unary minus", "- expr", 1, 2, Interaction::OperatorDescriptor::RightAssociative));
	instance()->addDescriptor(new UnaryOperatorDescriptor(OOModel::UnaryOperation::NOT,
			"not", "! expr", 1, 2, Interaction::OperatorDescriptor::RightAssociative));
	instance()->addDescriptor(new UnaryOperatorDescriptor(OOModel::UnaryOperation::COMPLEMENT,
			"complement", "~ expr", 1, 2, Interaction::OperatorDescriptor::RightAssociative));

	// Binary operators
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::TIMES,
			"times", "expr * expr", 2, 3, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::DIVIDE,
			"divide", "expr / expr", 2, 3, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::REMAINDER,
			"remainder", "expr % expr", 2, 3, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::PLUS,
			"plus", "expr + expr", 2, 4, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::MINUS,
			"minus", "expr - expr", 2, 4, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::LEFT_SHIFT,
			"left shift", "expr << expr", 2, 5, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::RIGHT_SHIFT_SIGNED,
			"right shift signed", "expr >> expr", 2, 5, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::RIGHT_SHIFT_UNSIGNED,
			"righ shift unsigned", "expr >>> expr", 2, 5, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::LESS,
			"less", "expr < expr", 2, 6, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::GREATER,
			"greater", "expr > expr", 2, 6, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::LESS_EQUALS,
			"less equals", "expr <= expr", 2, 6, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::GREATER_EQUALS,
			"greater equals", "expr >= expr", 2, 6, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::EQUALS,
			"equals", "expr == expr", 2, 7, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::NOT_EQUALS,
			"not equals", "expr != expr", 2, 7, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::XOR,
			"bitwise xor", "expr ^ expr", 2, 9, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::AND,
			"bitwise and", "expr & expr", 2, 8, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::OR,
			"bitwise or", "expr | expr", 2, 10, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::CONDITIONAL_AND,
			"conditional and", "expr && expr", 2, 11, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::CONDITIONAL_OR,
			"conditional or", "expr || expr", 2, 12, Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new BinaryOperatorDescriptor(OOModel::BinaryOperation::ARRAY_INDEX,
			"array index", "expr [ expr ]", 2, 1, Interaction::OperatorDescriptor::LeftAssociative));

	// Others
	instance()->addDescriptor(new CastDescriptor( "cast", "( expr ) expr", 2, 2,
			Interaction::OperatorDescriptor::RightAssociative));
	instance()->addDescriptor(new CommaDescriptor( "comma", "expr , expr", 2, 50,
			Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new InitializerDescriptor( "initializer", "{ expr }", 1, 0,
			Interaction::OperatorDescriptor::NotAssociative));
	instance()->addDescriptor(new NewArrayDescriptor( "new array", "new expr [ expr ]", 2, 2,
			Interaction::OperatorDescriptor::RightAssociative));
	instance()->addDescriptor(new MemberOperatorDescriptor( "member", "expr . id", 2, 1,
			Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new CallDescriptor( "call", "expr ( expr )", 2, 1,
			Interaction::OperatorDescriptor::LeftAssociative));
	instance()->addDescriptor(new ArrayTypeDescriptor( "array type", "expr []", 1, 1,
			Interaction::OperatorDescriptor::LeftAssociative));
}

} /* namespace OOInteraction */
