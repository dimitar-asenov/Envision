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

#include "string_components/AssignmentStringComponents.h"

#include "OOModel/src/expressions/AssignmentExpression.h"

namespace OOInteraction {

AssignmentStringComponents::AssignmentStringComponents(OOModel::AssignmentExpression* e )
	: exp_(e)
{
}

QStringList AssignmentStringComponents::components()
{
	QStringList result;
	if (!exp_) return result;

	// First comes the prefix. Currently there is no assignment operator that has a prefix.
	result << QString() << stringForNode(exp_->left());

	switch(exp_->op())
	{
		case OOModel::AssignmentExpression::ASSIGN: result << "="; break;
		case OOModel::AssignmentExpression::PLUS_ASSIGN: result << "+="; break;
		case OOModel::AssignmentExpression::MINUS_ASSIGN: result << "-="; break;
		case OOModel::AssignmentExpression::TIMES_ASSIGN: result << "*="; break;
		case OOModel::AssignmentExpression::DIVIDE_ASSIGN: result << "/="; break;
		case OOModel::AssignmentExpression::BIT_AND_ASSIGN: result << "&="; break;
		case OOModel::AssignmentExpression::BIT_OR_ASSIGN: result << "|="; break;
		case OOModel::AssignmentExpression::BIT_XOR_ASSIGN: result << "^="; break;
		case OOModel::AssignmentExpression::REMAINDER_ASSIGN: result << "%="; break;
		case OOModel::AssignmentExpression::LEFT_SHIFT_ASSIGN: result << "<<="; break;
		case OOModel::AssignmentExpression::RIGHT_SHIFT_SIGNED_ASSIGN: result << ">>="; break;
		case OOModel::AssignmentExpression::RIGHT_SHIFT_UNSIGNED_ASSIGN: result << ">>>="; break;
		default: result.append( QString() ); break;
	}

	result << stringForNode(exp_->right()) << QString(); // Postfix

	return result;
}

} /* namespace OOInteraction */
