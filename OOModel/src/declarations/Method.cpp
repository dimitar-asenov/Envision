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

#include "Method.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::Method)

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(Method)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(Method)

REGISTER_ATTRIBUTE(Method, items, StatementItemList, false, false, true)
REGISTER_ATTRIBUTE(Method, typeArguments, TypedListOfFormalTypeArgument, false, false, true)
REGISTER_ATTRIBUTE(Method, arguments, TypedListOfFormalArgument, false, false, true)
REGISTER_ATTRIBUTE(Method, results, TypedListOfFormalResult, false, false, true)
REGISTER_ATTRIBUTE(Method, memberInitializers, TypedListOfMemberInitializer, false, true, true)
REGISTER_ATTRIBUTE(Method, storageSpecifier, StorageSpecifier, false, false, true)
REGISTER_ATTRIBUTE(Method, mthKind, Integer, false, false, true)

Method::Method(const QString& name) : Super(nullptr, Method::getMetaData())
{
	setName(name);
}

Method::Method(const QString& name, Visibility::VisibilityType vis) : Super(nullptr, Method::getMetaData())
{
	setName(name);
	setVisibility(vis);
}

Method::Method(const QString& name, StorageSpecifier::StorageSpecifierTypes storage)
: Super(nullptr, Method::getMetaData())
{
	setName(name);
	setStorageSpecifier(storage);
}

Method::Method(const QString& name, Visibility::VisibilityType vis, StorageSpecifier::StorageSpecifierTypes storage)
: Super(nullptr, Method::getMetaData())
{
	setName(name);
	setVisibility(vis);
	setStorageSpecifier(storage);
}

Method::Method(const QString& name, MethodKind kind) : Super(nullptr, Method::getMetaData())
{
	setName(name);
	setMethodKind(kind);
}

Method::Method(const QString& name, Visibility::VisibilityType vis, MethodKind kind)
: Super(nullptr, Method::getMetaData())
{
	setName(name);
	setVisibility(vis);
	setMethodKind(kind);
}

Method::Method(const QString& name, StorageSpecifier::StorageSpecifierTypes storage, MethodKind kind)
: Super(nullptr, Method::getMetaData())
{
	setName(name);
	setStorageSpecifier(storage);
	setMethodKind(kind);
}

Method::Method(const QString& name, Visibility::VisibilityType vis,
					StorageSpecifier::StorageSpecifierTypes storage, MethodKind kind)
: Super(nullptr, Method::getMetaData())
{
	setName(name);
	setVisibility(vis);
	setStorageSpecifier(storage);
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

QList<Model::Node*> Method::findSymbols(const QRegExp& symbolExp, Model::Node* source, FindSymbolMode mode,
		bool exhaustAllScopes)
{
	if (mode == SEARCH_UP && isAncestorOf(source))
	{
		QList<Model::Node*> symbols;

		symbols << arguments()->findAllSymbolDefinitions(symbolExp);
		symbols << results()->findAllSymbolDefinitions(symbolExp);
		// Note that a StatementList also implements findSymbols and locally declared variables will be found there.

		if (exhaustAllScopes || symbols.isEmpty())
			symbols << Node::findSymbols(symbolExp, source, mode, exhaustAllScopes);

		return symbols;
	}
	else return QList<Model::Node*> ();
}

bool Method::isGeneric()
{
	return typeArguments()->size() > 0;
}

}
