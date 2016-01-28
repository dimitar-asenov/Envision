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

#include "Class.h"

#include "../types/SymbolProviderType.h"
#include "../types/ClassType.h"
#include "Project.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<OOModel::Class>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(Class)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(Class)

REGISTER_ATTRIBUTE(Class, baseClasses, TypedListOfExpression, false, false, true)
REGISTER_ATTRIBUTE(Class, friends, TypedListOfDeclaration, false, false, true)
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

inline bool Class::findInTarget(Expression* target, QSet<Node*>& result, const Model::SymbolMatcher& matcher,
		SymbolTypes symbolTypes, bool exhaustAllScopes) const
{
	bool found = false;
	auto t = target->type();
	if (auto sp = dynamic_cast<SymbolProviderType*>(t.get()))
	{

		if (sp->symbolProvider() && sp->symbolProvider() != this)
			found = sp->symbolProvider()->findSymbols(result, matcher, sp->symbolProvider(), SEARCH_DOWN,
				symbolTypes, exhaustAllScopes);
	}
	return found;
}

bool Class::findSymbols(QSet<Node*>& result, const Model::SymbolMatcher& matcher, const Node* source,
				FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const
{
	bool found{};

	if (direction == SEARCH_HERE)
	{
		if (symbolMatches(matcher, symbolTypes))
		{
			found = true;
			result.insert(const_cast<Class*>(this));
		}
	}
	else if (direction == SEARCH_DOWN)
	{
		for (auto c : childrenInScope())
		{
			if (c != typeArguments() && c != friends())
				found = c->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;
		}

		// Look in base classes
		if (exhaustAllScopes || !found)
		{
			if (baseClasses()->size() == 0)
			{
				if (auto implicit = defaultImplicitBaseFromProject())
					found = findInTarget(implicit, result, matcher, symbolTypes, exhaustAllScopes) || found;
			}
			else
				for (auto bc : *baseClasses())
					found = findInTarget(bc, result, matcher, symbolTypes, exhaustAllScopes) || found;
		}
	}
	else if (direction == SEARCH_UP)
	{
		auto ignore = childToSubnode(source);
		for (auto c : childrenInScope())
			if (c != ignore && c != friends())
				// Optimize the search by skipping this scope, since we've already searched there
				found = c->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;

		// Look in base classes, but only if we are not trying to resolve a base class name
		ignore = baseClasses()->childToSubnode(source);
		if (!ignore && (exhaustAllScopes || !found))
		{
			if (baseClasses()->size() == 0)
			{
				if (auto implicit = defaultImplicitBaseFromProject())
					found = findInTarget(implicit, result, matcher, symbolTypes, exhaustAllScopes) || found;
			}
			else
				for (auto bc : *baseClasses())
					found = findInTarget(bc, result, matcher, symbolTypes, exhaustAllScopes) || found;
		}

		if ((exhaustAllScopes || !found) && symbolMatches(matcher, symbolTypes))
		{
			found = true;
			result.insert(const_cast<Class*>(this));
		}

		if ((exhaustAllScopes || !found) && parent())
			found = parent()->findSymbols(result, matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes) || found;
	}

	return found;
}

Expression* Class::defaultImplicitBaseFromProject() const
{
	auto p = parent();
	while (p)
	{
		if (auto proj = DCast<Project>(p))
		{
			if (constructKind() != ConstructKind::Enum && proj->implicitBaseType()) return proj->implicitBaseType();
			if (constructKind() == ConstructKind::Enum && proj->implicitEnumType()) return proj->implicitEnumType();
		}
		p = p->parent();
	}
	return nullptr;
}

QSet<Class*> Class::allBaseClasses()
{
	QSet<Class*> bases;

	for (auto base : *baseClasses())
	{
		auto type = base->type();
		auto ct = dynamic_cast<ClassType*>(type.get());
		if (ct)
		{
			bases.insert(ct->classDefinition());
			bases.unite(ct->classDefinition()->allBaseClasses());
		}
	}

	if (baseClasses()->isEmpty())
	{
		if (auto classDef = implicitBaseFromProject())
		{
				bases.insert(classDef);
				bases.unite(classDef->allBaseClasses());
		}
	}

	return bases;
}

QSet<Class*> Class::directBaseClasses()
{
	QSet<Class*> result;
	for (auto baseClass : *baseClasses())
		if (auto asClass = expressionToClass(baseClass))
			result << asClass;
	if (result.isEmpty() && implicitBaseFromProject())
		result << implicitBaseFromProject();
	return result;
}

Class* Class::implicitBaseFromProject() const
{
	if (auto classDef = expressionToClass(defaultImplicitBaseFromProject()))
	{
		if (this != classDef) return classDef;
	}

	return nullptr;
}

Class* Class::expressionToClass(Expression* expr)
{
	if (expr)
	{
		auto type = std::unique_ptr<Type>(expr->type());
		if (auto ct = dynamic_cast<ClassType*>(type.get()))
			return ct->classDefinition();
	}
	return nullptr;
}

QSet<Class*> Class::directSubClasses()
{
	auto top = root();
	//Find all the classes where this method is subclasses
	QSet<Class*> result;
	QList<Model::Node*> toCheck;
	toCheck.append(top);
	while (!toCheck.isEmpty())
	{
		auto check = toCheck.takeLast();
		if (auto someClass = DCast<Class>(check))
			if (someClass->directBaseClasses().contains(this))
				result << someClass;
		toCheck.append(check->children());
	}
	return result;
}

bool Class::isNewPersistenceUnit() const
{
	return true;
}

}
