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

#include "expressions/CommaExpression.h"

#include "expressions/EmptyExpression.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::CommaExpression)

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommaExpression)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommaExpression)

REGISTER_ATTRIBUTE(CommaExpression, left, Expression, false, false, true)
REGISTER_ATTRIBUTE(CommaExpression, right, Expression, false, false, true)

CommaExpression::CommaExpression(Expression* left, Expression* right)
: Super(nullptr, CommaExpression::getMetaData())
{
	if(left) setLeft(left);
	if(right) setRight(right);
}

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

bool CommaExpression::findSymbols(QSet<Node*>& result, const Model::SymbolMatcher& matcher, const Node* source,
		FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const
{
	Q_ASSERT(direction != SEARCH_DOWN);

	bool found{};

	if (direction == SEARCH_HERE)
	{
		found = left()->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;
		found = right()->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;
	}
	else if (direction == SEARCH_UP)
	{
		auto ignore = childToSubnode(source);

		if (left() != ignore)
			found = left()->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;
		if (right() != ignore)
			found = right()->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;

		if ((exhaustAllScopes || !found) && parent())
			found = parent()->findSymbols(result, matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes) || found;
	}

	return found;
}

Type* CommaExpression::type()
{
	return right()->type();
}

}
