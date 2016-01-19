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

#include "PrimitiveType.h"

namespace OOModel {

PrimitiveType::PrimitiveType(PrimitiveTypes type, bool isValueType) : Type{isValueType}, type_{type}
{
}

bool PrimitiveType::equals(const Type* other) const
{
	if (auto pt = dynamic_cast<const PrimitiveType*>(other))
		return type_ == pt->type_ && qualifiers() == other->qualifiers();

	return false;
}

PrimitiveType::PrimitiveTypes PrimitiveType::resultFromBinaryOperation(PrimitiveTypes left, PrimitiveTypes right)
{
	if (left == right) return left;
	if (left == DOUBLE || right == DOUBLE) return DOUBLE;
	if (left == FLOAT || right == FLOAT) return FLOAT;
	if ( (left == LONG || right == LONG)
			&& left != UNSIGNED_LONG && left != UNSIGNED_INT && right != UNSIGNED_LONG && right != UNSIGNED_INT)
		return LONG;
	if ( (left == UNSIGNED_LONG || right == UNSIGNED_LONG)
			&& left != LONG && left != INT && right != LONG && right != INT)
		return UNSIGNED_LONG;

	return VOID;
}

PrimitiveType* PrimitiveType::clone() const
{
	return new PrimitiveType{*this};
}

}
