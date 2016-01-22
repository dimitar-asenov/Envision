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

#include "TypeAlias.h"
#include "../types/SymbolProviderType.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
template class Model::TypedList<OOModel::TypeAlias>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(TypeAlias)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(TypeAlias)

REGISTER_ATTRIBUTE(TypeAlias, typeExpression, Expression, false, false, true)
REGISTER_ATTRIBUTE(TypeAlias, typeArguments, TypedListOfFormalTypeArgument, false, false, true)

TypeAlias::TypeAlias(const QString &name, Expression *typeExpression)
: Super(nullptr, TypeAlias::getMetaData())
{
	setName(name);
	if (typeExpression) setTypeExpression(typeExpression);
}

Model::Node* TypeAlias::target() const
{
	Model::Node* ret{};

	auto type = this->typeExpression()->type();
	if (auto sp = dynamic_cast<SymbolProviderType*>(type))
		ret = sp->symbolProvider();

	SAFE_DELETE(type);

	return ret;
}

TypeAlias::SymbolTypes TypeAlias::symbolType() const
{
	if (auto t = target()) return t->symbolType();
	else return UNSPECIFIED;
}

bool TypeAlias::findSymbols(QSet<Node*>& result, const Model::SymbolMatcher& matcher, const Model::Node* source,
		FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const
{
	// TODO: Search type arguments

	if (direction == SEARCH_HERE)
	{
		if (symbolMatches(matcher, symbolTypes))
		{
			result.insert(const_cast<TypeAlias*>(this));
			return true;
		}
	}
	else if (direction == SEARCH_DOWN)
	{
		if (auto t = target())
			return t->findSymbols(result, matcher, t, SEARCH_DOWN, symbolTypes, false);
	}
	else if (direction == SEARCH_UP)
	{
		if (parent())
			return parent()->findSymbols(result, matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes);
	}

	return false;
}

}
