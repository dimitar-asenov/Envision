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

#include "string_components/PrimitiveTypeStringComponents.h"
#include "OOInteractionException.h"

#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"

namespace OOInteraction {

PrimitiveTypeStringComponents::PrimitiveTypeStringComponents(OOModel::PrimitiveTypeExpression* e)
	: exp_(e)
{
}

QStringList PrimitiveTypeStringComponents::components()
{
	QStringList result;
	if (!exp_) return result;

	switch(exp_->typeValue())
	{
		case OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT : result << "int"; break;
		case OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG : result << "long"; break;
		case OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT : result << "uint"; break;
		case OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG : result << "ulong"; break;
		case OOModel::PrimitiveTypeExpression::PrimitiveTypes::FLOAT : result << "float"; break;
		case OOModel::PrimitiveTypeExpression::PrimitiveTypes::DOUBLE : result << "double"; break;
		case OOModel::PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN : result << "bool"; break;
		case OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR : result << "char"; break;
		case OOModel::PrimitiveTypeExpression::PrimitiveTypes::VOID : result << "void"; break;
		default: throw OOInteractionException("Unknown primitive type");
	}
	return result;
}

} /* namespace OOInteraction */
