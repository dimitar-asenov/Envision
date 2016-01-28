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

#include "NameImport.h"
#include "../types/SymbolProviderType.h"
#include "../expressions/ReferenceExpression.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<OOModel::NameImport>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(NameImport)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(NameImport)

REGISTER_ATTRIBUTE(NameImport, importedName, Expression, false, false, true)
REGISTER_ATTRIBUTE(NameImport, importAll, Boolean, false, false, true)

NameImport::NameImport(Expression *importedName, bool importAllChildrenInScope)
: Super(nullptr, NameImport::getMetaData())
{
	if (importedName) setImportedName(importedName);
	if (importAllChildrenInScope) setImportAll(true);
}

bool NameImport::definesSymbol() const
{
	return false;
}

Model::Node* NameImport::target() const
{
	Model::Node* ret{};

	auto type = this->importedName()->type();
	if (auto sp = dynamic_cast<SymbolProviderType*>(type.get()))
		ret = sp->symbolProvider();

	return ret;
}

bool NameImport::findSymbols(QSet<Node*>& result, const Model::SymbolMatcher& matcher, const Model::Node* source,
		FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const
{
	Q_ASSERT(direction != SEARCH_DOWN);

	// Name imports only provide shortcuts for objects that are within the entity declaring the import
	auto p = parent(); // This should be a list of Declarations
	if (p) p = p->parent(); // This should be the parent entity
	if (!p || !p->isAncestorOf(source)) return false;
	// Note above that it is important that we only consider descendants of p and not p itself. This is because when
	// a NameImport (or the initial part of one) within p, resolves to p itself (e.g. import java.something inside the
	// java package) we will do a symbol start with p as a source. In that case import should not be further used as
	// shortcuts.


	if (direction == SEARCH_HERE)
	{
		// If this node is part of a list and the source is a name import from the same list, impose an order
		auto listParent = DCast<Model::List>(parent());
		if (listParent)
		{
			int sourceIndex = listParent->indexToSubnode(source);
			if (sourceIndex >=0)
				if (DCast<NameImport> (listParent->at<Model::Node>(sourceIndex)))
				{
					int thisIndex = listParent->indexOf(this);
					if (thisIndex > sourceIndex)
						return false;
				}
		}

		// If this node defines a shortcut to a single name which is not the one being looked for, then do not resolve
		// the target
		if (!importAll())
			if (auto ref = DCast<ReferenceExpression>(importedName()))
				if (!matcher.matches(ref->name())) return false;

		if (auto t = target())
			return t->findSymbols(result, matcher, (importAll() ? t : source),
					(importAll() ? SEARCH_DOWN : SEARCH_HERE), symbolTypes, false);
	}
	else if (direction == SEARCH_UP)
	{
		if (parent())
			return parent()->findSymbols(result, matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes);
	}

	return false;
}

}
