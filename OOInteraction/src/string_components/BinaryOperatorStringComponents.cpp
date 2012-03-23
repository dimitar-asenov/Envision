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
 * BinaryOperatorStringComponents.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: Dimitar Asenov
 */

#include "string_components/BinaryOperatorStringComponents.h"

#include "OOModel/src/expressions/BinaryOperation.h"

namespace OOInteraction {

BinaryOperatorStringComponents::BinaryOperatorStringComponents(OOModel::BinaryOperation* e )
	: exp_(e)
{
}

QStringList BinaryOperatorStringComponents::components()
{
	QStringList result;
	if (!exp_) return result;

	// First comes the prefix. Currently there is no binary operator that has a prefix.
	result << "" << stringForNode(exp_->left());

	switch(exp_->op())
	{
		case OOModel::BinaryOperation::TIMES: result << "*"; break;
		case OOModel::BinaryOperation::DIVIDE: result << "/"; break;
		case OOModel::BinaryOperation::REMAINDER: result << "%"; break;
		case OOModel::BinaryOperation::PLUS: result << "+"; break;
		case OOModel::BinaryOperation::MINUS: result << "-"; break;
		case OOModel::BinaryOperation::LEFT_SHIFT: result << "<<"; break;
		case OOModel::BinaryOperation::RIGHT_SHIFT_SIGNED: result << ">>"; break;
		case OOModel::BinaryOperation::RIGHT_SHIFT_UNSIGNED: result << ">>>"; break;
		case OOModel::BinaryOperation::LESS: result << "<"; break;
		case OOModel::BinaryOperation::GREATER: result << ">"; break;
		case OOModel::BinaryOperation::LESS_EQUALS: result << "<="; break;
		case OOModel::BinaryOperation::GREATER_EQUALS: result << ">="; break;
		case OOModel::BinaryOperation::EQUALS: result << "=="; break;
		case OOModel::BinaryOperation::NOT_EQUALS: result << "!="; break;
		case OOModel::BinaryOperation::XOR: result << "^"; break;
		case OOModel::BinaryOperation::AND: result << "&"; break;
		case OOModel::BinaryOperation::OR: result << "|"; break;
		case OOModel::BinaryOperation::CONDITIONAL_AND: result << "&&"; break;
		case OOModel::BinaryOperation::CONDITIONAL_OR: result << "||"; break;
		case OOModel::BinaryOperation::ARRAY_INDEX: result << "["; break;
		default: result << QString(); break;
	}

	result << stringForNode(exp_->right());
	result << (exp_->op() == OOModel::BinaryOperation::ARRAY_INDEX ? "]" : QString()); // Postfix

	return result;
}

} /* namespace OOInteraction */
