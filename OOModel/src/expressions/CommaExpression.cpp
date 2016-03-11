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

#include "CommaExpression.h"
#include "EmptyExpression.h"
#include "../types/Type.h"

#include "ModelBase/src/nodes/TypedList.hpp"
#include "OOModel/src/typesystem/OOResolutionRequest.h"
template class Model::TypedList<OOModel::CommaExpression>;

namespace OOModel {

DEFINE_COMPOSITE_EMPTY_CONSTRUCTORS(CommaExpression)
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(CommaExpression)

DEFINE_ATTRIBUTE(CommaExpression, left, Expression, false, false, true)
DEFINE_ATTRIBUTE(CommaExpression, right, Expression, false, false, true)

CommaExpression::CommaExpression(Expression* left, Expression* right)
: Super{nullptr, CommaExpression::getMetaData()}
{
	if (left) setLeft(left);
	if (right) setRight(right);
}

QList<Expression*> CommaExpression::allSubOperands(bool detachOperands)
{
	QList<Expression*> operands;

	if (auto left_comma = DCast<CommaExpression>(left()))
		operands.append(left_comma->allSubOperands(detachOperands));
	else
	{
		operands.append(left());
		if (detachOperands) replaceChild(left(), new EmptyExpression{});
	}

	if (auto right_comma = DCast<CommaExpression>(right()))
		operands.append(right_comma->allSubOperands(detachOperands));
	else
	{
		operands.append(right());
		if (detachOperands) replaceChild(right(), new EmptyExpression{});
	}
	return operands;
}

bool CommaExpression::findSymbols(std::unique_ptr<Model::ResolutionRequest> request) const
{
	Q_ASSERT(request->direction() != SEARCH_DOWN);

	bool found{};

	if (request->direction() == SEARCH_HERE)
	{
		found = left()->findSymbols(request->clone(SEARCH_HERE, false)) || found;
		found = right()->findSymbols(request->clone(SEARCH_HERE, false)) || found;
	}
	else if (request->direction() == SEARCH_UP || request->direction() == SEARCH_UP_ORDERED)
	{
		auto ignore = childToSubnode(request->source());

		if (left() != ignore)
			found = left()->findSymbols(request->clone(SEARCH_HERE, false)) || found;
		if (right() != ignore)
			found = right()->findSymbols(request->clone(SEARCH_HERE, false)) || found;

		if ((request->exhaustAllScopes() || !found) && parent())
			found = parent()->findSymbols(request->clone(SEARCH_UP)) || found;
	}

	return found;
}

std::unique_ptr<Type> CommaExpression::type(const TypeArgumentBindings& typeArgumentBindings)
{
	return right()->type(typeArgumentBindings);
}

}
