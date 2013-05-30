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

#include "expressions/CommaExpression.h"

#include "expressions/EmptyExpression.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::CommaExpression)

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommaExpression)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommaExpression)

REGISTER_ATTRIBUTE(CommaExpression, left, Expression, false, false, true)
REGISTER_ATTRIBUTE(CommaExpression, right, Expression, false, false, true)

QList<Expression*> CommaExpression::allSubOperands(bool detachOperands)
{
	QList<Expression*> operands;

	if (auto left_comma = dynamic_cast<CommaExpression*>(left()))
		operands.append(left_comma->allSubOperands(detachOperands));
	else
	{
		operands.append(left());
		if (detachOperands) replaceChild(left(), new EmptyExpression());
	}

	if (auto right_comma = dynamic_cast<CommaExpression*>(right()))
		operands.append(right_comma->allSubOperands(detachOperands));
	else
	{
		operands.append(right());
		if (detachOperands) replaceChild(right(), new EmptyExpression());
	}
	return operands;
}

QList<Model::Node*> CommaExpression::findSymbols(const QRegExp& symbolExp, Node* source, FindSymbolMode mode,
		bool exhaustAllScopes)
{
	if (mode == SEARCH_DOWN)
	{
		QList<Model::Node*> symbols;
		symbols << left()->findSymbols(symbolExp, source, mode, exhaustAllScopes);
		symbols << right()->findSymbols(symbolExp, source, mode, exhaustAllScopes);

		return symbols;
	}
	else return Expression::findSymbols(symbolExp, source, mode, exhaustAllScopes);
}

Type* CommaExpression::type()
{
	return right()->type();
}

}
