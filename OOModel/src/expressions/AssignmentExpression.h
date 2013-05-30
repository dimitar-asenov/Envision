/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#pragma once

#include "Expression.h"

#include "ModelBase/src/nodes/Integer.h"

DECLARE_TYPED_LIST(OOMODEL_API, OOModel, AssignmentExpression)

namespace OOModel {

class OOMODEL_API AssignmentExpression: public Super<Expression>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(AssignmentExpression)

	ATTRIBUTE(Expression, left, setLeft)
	ATTRIBUTE(Expression, right, setRight)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, opr, setOpr, int)

	public:
		enum AssignmentTypes { ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, TIMES_ASSIGN, DIVIDE_ASSIGN, BIT_AND_ASSIGN,
			BIT_OR_ASSIGN, BIT_XOR_ASSIGN, REMAINDER_ASSIGN, LEFT_SHIFT_ASSIGN, RIGHT_SHIFT_SIGNED_ASSIGN,
			RIGHT_SHIFT_UNSIGNED_ASSIGN};

		AssignmentTypes op() const;
		void setOp(const AssignmentTypes& oper);

		virtual Type* type();
};

inline AssignmentExpression::AssignmentTypes AssignmentExpression::op() const { return static_cast<AssignmentTypes> (opr()); }
inline void AssignmentExpression::setOp(const AssignmentTypes& oper) { setOpr(oper); }

}
