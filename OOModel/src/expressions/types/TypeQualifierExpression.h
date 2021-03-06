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

#pragma once

#include "TypeExpression.h"
#include "../../types/Type.h"

#include "ModelBase/src/nodes/Integer.h"

namespace OOModel {
	class TypeQualifierExpression;
}
extern template class OOMODEL_API Model::TypedList<OOModel::TypeQualifierExpression>;

namespace OOModel {

class OOMODEL_API TypeQualifierExpression : public Super<TypeExpression>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(TypeQualifierExpression)

	ATTRIBUTE(Expression, typeExpression, setTypeExpression)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, qualifierVal, setQualifierVal, int)

	public:
		using Qualifier = Type::Qualifier;

		TypeQualifierExpression(Qualifier q, Expression* e = nullptr);

		Qualifier qualifier() const;
		void setQualifier(Qualifier q);

		virtual std::unique_ptr<Type> type() override;
};

inline TypeQualifierExpression::Qualifier TypeQualifierExpression::qualifier()
	const { return static_cast<Qualifier> (qualifierVal()); }
inline void TypeQualifierExpression::setQualifier(Qualifier q){ setQualifierVal(q);}

}
