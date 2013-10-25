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

#include "Class.h"

#include "../types/SymbolProviderType.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::Class)

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(Class)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(Class)

REGISTER_ATTRIBUTE(Class, baseClasses, TypedListOfExpression, false, false, true)
REGISTER_ATTRIBUTE(Class, friends, TypedListOfExpression, false, false, true)
REGISTER_ATTRIBUTE(Class, typeArguments, TypedListOfFormalTypeArgument, false, false, true)
REGISTER_ATTRIBUTE(Class, classes, TypedListOfClass, false, false, true)
REGISTER_ATTRIBUTE(Class, methods, TypedListOfMethod, false, false, true)
REGISTER_ATTRIBUTE(Class, fields, TypedListOfField, false, false, true)
REGISTER_ATTRIBUTE(Class, enumerators, TypedListOfEnumerator, false, false, true)
REGISTER_ATTRIBUTE(Class, cKind, Integer, false, false, true)

Class::Class(const QString& name)
: Super(nullptr, Class::getMetaData())
{
	setName(name);
	setConstructKind(ConstructKind::Class);
}

Class::Class(const QString& name, ConstructKind kind)
: Super(nullptr, Class::getMetaData())
{
	setName(name);
	setConstructKind(kind);
}

Class::Class(const QString& name, Modifier::Modifiers mod, ConstructKind kind)
: Super(nullptr, Class::getMetaData())
{
	setName(name);
	modifiers()->set(mod);
	setConstructKind(kind);
}

Class::SymbolTypes Class::symbolType() const
{
	return CONTAINER;
}

bool Class::isGeneric()
{
	return typeArguments()->size() > 0;
}

bool Class::findSymbols(QSet<Node*>& result, const Model::SymbolMatcher& matcher, Node* source,
				FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes)
{
	bool found{};

	if (direction == SEARCH_HERE)
	{
		if (symbolMatches(matcher, symbolTypes))
		{
			found = true;
			result.insert(this);
		}
	}
	else if (direction == SEARCH_DOWN)
	{
		for (auto c : childrenInScope())
			found = c->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;

		// Look in base classes
		if (exhaustAllScopes || !found)
			for(auto bc : *baseClasses())
			{
				auto t = bc->type();
				if (auto sp = dynamic_cast<SymbolProviderType*>(t))
				{
					if (sp->symbolProvider())
						found = sp->symbolProvider()->findSymbols(result, matcher, sp->symbolProvider(), SEARCH_DOWN,
							symbolTypes, false) || found;

				}
				SAFE_DELETE(t);
			}
	}
	else if (direction == SEARCH_UP)
	{
		auto ignore = childToSubnode(source);
		for (auto c : childrenInScope())
			if (c != ignore) // Optimize the search by skipping this scope, since we've already searched there
				found = c->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;

		// Look in base classes, but only if we are not trying to resolve a base class name
		ignore = baseClasses()->childToSubnode(source);
		if (!ignore && (exhaustAllScopes || !found))
			for(auto bc : *baseClasses())
			{
				auto t = bc->type();
				if (auto sp = dynamic_cast<SymbolProviderType*>(t))
				{
					if (sp->symbolProvider())
						found = sp->symbolProvider()->findSymbols(result, matcher, sp->symbolProvider(), SEARCH_DOWN,
							symbolTypes, false) || found;

				}
				SAFE_DELETE(t);
			};

		if ((exhaustAllScopes || !found) && symbolMatches(matcher, symbolTypes))
		{
			found = true;
			result.insert(this);
		}

		if ((exhaustAllScopes || !found) && parent())
			found = parent()->findSymbols(result, matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes) || found;
	}

	return found;
}

}
