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

namespace OOModel { class CastExpression; }
extern template class OOMODEL_API Model::TypedList<OOModel::CastExpression>;

namespace OOModel {

class OOMODEL_API CastExpression: public Super<Expression>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(CastExpression)

	ATTRIBUTE(Expression, castType, setType)
	ATTRIBUTE(Expression, expr, setExpr)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, cKind, setCKind, int)

	public:
		virtual std::unique_ptr<Type> type() override;

		enum class CastKind : int
		{
			Default,
			ConstCast,
			DynamicCast,
			ReinterpretCast,
			StaticCast,
			/**
			 * We currently do not use this type of cast when importing C++ code.
			 * Functional casts are converted to construction method calls instead.
			 */
			FunctionalCast
		};

		CastExpression(CastKind castKind);

		CastKind castKind() const;
		void setCastKind(const CastKind& castKind);

};

inline CastExpression::CastKind CastExpression::castKind() const {return static_cast<CastKind> (cKind()); }
inline void CastExpression::setCastKind(const CastKind& castKind) { setCKind(static_cast<int> (castKind)); }

}
