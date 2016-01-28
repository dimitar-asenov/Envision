/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

namespace OOModel { class TypeTraitExpression; }
extern template class OOMODEL_API Model::TypedList<OOModel::TypeTraitExpression>;

namespace OOModel {

class OOMODEL_API TypeTraitExpression : public Super<Expression>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(TypeTraitExpression)

	ATTRIBUTE(Expression, operand, setOperand)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, ttKind, setTTKind, int)

	public:
		enum class TypeTraitKind : int {SizeOf, AlignOf, TypeId};

		TypeTraitExpression(TypeTraitKind kind, Expression* expr = nullptr);

		TypeTraitKind typeTraitKind() const;
		void setTypeTraitKind(const TypeTraitKind& kind);

		virtual std::unique_ptr<Type> type() override;
};

inline TypeTraitExpression::TypeTraitKind TypeTraitExpression::typeTraitKind() const
{ return static_cast<TypeTraitKind>(ttKind()); }

inline void TypeTraitExpression::setTypeTraitKind(const TypeTraitKind &kind)
{ setTTKind(static_cast<int>(kind)); }

}
