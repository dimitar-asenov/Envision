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

#include "Method.h"
#include "Class.h"

#include "ModelBase/src/nodes/TypedList.hpp"
#include "../types/Type.h"
#include "../expressions/MethodCallExpression.h"

#include "OOModel/src/typesystem/OOResolutionRequest.h"
template class Model::TypedList<OOModel::Method>;

namespace OOModel {

DEFINE_COMPOSITE_EMPTY_CONSTRUCTORS(Method)
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(Method)

DEFINE_ATTRIBUTE(Method, items, StatementItemList, true, false, true)
DEFINE_ATTRIBUTE(Method, typeArguments, TypedListOfFormalTypeArgument, false, false, true)
DEFINE_ATTRIBUTE(Method, arguments, TypedListOfFormalArgument, false, false, true)
DEFINE_ATTRIBUTE(Method, results, TypedListOfFormalResult, false, false, true)
DEFINE_ATTRIBUTE(Method, memberInitializers, TypedListOfMemberInitializer, false, false, true)
DEFINE_ATTRIBUTE(Method, throws, TypedListOfExpression, false, false, true)
DEFINE_ATTRIBUTE(Method, mthKind, Integer, false, false, true)

Method::Method(const QString& name) : Super{nullptr, Method::getMetaData()}
{
	setName(name);
}

Method::Method(const QString& name, Modifier::Modifiers mod) : Super{nullptr, Method::getMetaData()}
{
	setName(name);
	modifiers()->set(mod);
}

Method::Method(const QString& name, MethodKind kind) : Super{nullptr, Method::getMetaData()}
{
	setName(name);
	setMethodKind(kind);
}

Method::Method(const QString& name, Modifier::Modifiers mod, MethodKind kind)
: Super{nullptr, Method::getMetaData()}
{
	setName(name);
	modifiers()->set(mod);
	setMethodKind(kind);
}

QString Method::fullyQualifiedName() const
{
	const Model::Node* n = this;
	QString result;

	// We omit the symbol of the top project name
	while (n && n->parent())
	{
		if (n->definesSymbol())
		{
			if (result.isEmpty()) result = n->symbolName();
			else result.prepend(n->symbolName() +".");
		}
		n = n->parent();
	}

	return result;
}

Method::SymbolTypes Method::symbolType() const
{
	return METHOD;
}

bool Method::findSymbols(std::unique_ptr<Model::ResolutionRequest> request) const
{
	bool found{};

	if (request->direction() == SEARCH_DOWN); // Do nothing
	else if (request->direction() == SEARCH_HERE)
	{
		if (symbolMatches(request->matcher(), request->symbolTypes()))
		{
			found = true;
			request->result().insert(const_cast<Method*>(this));
		}
	}
	else if (request->direction() == SEARCH_UP || request->direction() == SEARCH_UP_ORDERED)
	{
		auto ignore = childToSubnode(request->source());

		Q_ASSERT(ignore);

		// Don't search in scopes we've already searched in
		if (arguments() != ignore)
			found = arguments()->findSymbols(request->clone(SEARCH_HERE, false)) || found;
		if (typeArguments() != ignore)
			found = typeArguments()->findSymbols(request->clone(SEARCH_HERE, false)) || found;
		if (results() != ignore)
			found = results()->findSymbols(request->clone(SEARCH_HERE, false)) || found;
		if (subDeclarations() != ignore)
			found = subDeclarations()->findSymbols(request->clone(SEARCH_HERE, false)) || found;
		// Note that a StatementList (the body) also implements findSymbols and locally declared variables will be
		// found there.

		if ((request->exhaustAllScopes() || !found) && symbolMatches(request->matcher(), request->symbolTypes()))
		{
			found = true;
			request->result().insert(const_cast<Method*>(this));
		}

		if ((request->exhaustAllScopes() || !found) && parent())
			found = parent()->findSymbols(request->clone(SEARCH_UP)) || found;
	}

	return found;
}

bool Method::isGeneric()
{
	return typeArguments()->size() > 0;
}

bool Method::overrides(Method* other, const TypeArgumentBindings& typeArgumentBindings)
{
	Q_ASSERT(other != this);
	Q_ASSERT(other);

	// First check whether the types of arguments match

	if (name() != other->name()) return false;

	// TODO: support variable arguments
	if (arguments()->size() != other->arguments()->size()) return false;

	// Arguments must be identical
	for (int i = 0; i< arguments()->size(); ++i)
	{
		auto t1 = arguments()->at(i)->typeExpression()->type(typeArgumentBindings);
		auto t2 = other->arguments()->at(i)->typeExpression()->type(typeArgumentBindings);

		bool equal = t1->relationTo(t2.get()).testFlag(TypeSystem::Equal);

		if (!equal) return false;
	}

	// Second check whether the class hierarchy matches
	auto p = parent();
	while (p)
	{
		if (auto cl = DCast<Class>(p))
		{
			for (auto baseClass : cl->allBaseClasses(typeArgumentBindings))
				if (baseClass->methods()->contains(other))
					return true;

			return false;
		}

		p = p->parent();
	}

	return false;
}

QSet<Method*> Method::callees(const TypeArgumentBindings& typeArgumentBindings)
{
	QSet<Method*> result;
	QList<Model::Node*> toCheck;
	toCheck.append(this);
	while (!toCheck.isEmpty())
	{
		auto current = toCheck.takeLast();
		if (auto call = DCast<MethodCallExpression>(current))
			if (auto c = call->methodDefinition(typeArgumentBindings))
				result << c;
		toCheck.append(current->children());
	}
	return result;
}

QSet<Method*> Method::callers(const TypeArgumentBindings& typeArgumentBindings)
{
	auto top = root();
	//Find all the places where this method is called
	QSet<Method*> result;
	Method* current{};
	QList<Model::Node*> toCheck;
	toCheck.append(top);
	while (!toCheck.isEmpty())
	{
		auto check = toCheck.takeLast();
		//Set the current method whenever we enter a new method
		if (auto method = DCast<Method>(check))
			current = method;
		if (auto call = DCast<MethodCallExpression>(check))
			if (call->methodDefinition(typeArgumentBindings) == this && current)
				result << current;
		toCheck.append(check->children());
	}
	return result;
}

bool Method::isNewPersistenceUnit() const
{
	return true;
}

}
