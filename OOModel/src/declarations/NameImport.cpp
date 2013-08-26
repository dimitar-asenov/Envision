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

#include "NameImport.h"
#include "../types/SymbolProviderType.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::NameImport)

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(NameImport)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(NameImport)

REGISTER_ATTRIBUTE(NameImport, importedName, Expression, false, false, true)

NameImport::NameImport(Expression *importedName)
: Super(nullptr, NameImport::getMetaData())
{
	if(importedName) setImportedName(importedName);
}

bool NameImport::definesSymbol() const
{
	return false;
}

Model::Node* NameImport::target() const
{
	Model::Node* ret{};

	auto type = const_cast<NameImport*>(this)->importedName()->type();
	if (auto sp = dynamic_cast<SymbolProviderType*>(type))
		ret = sp->symbolProvider();

	SAFE_DELETE(type);

	return ret;
}

QSet<Model::Node*> NameImport::findSymbols(const Model::SymbolMatcher& matcher, Model::Node* source,
		FindSymbolDirection direction, SymbolTypes symbolTypes,bool exhaustAllScopes)
{
	Q_ASSERT(direction != SEARCH_DOWN);

	if (direction == SEARCH_HERE)
	{
		if (auto t = target())
			return t->findSymbols(matcher, source, SEARCH_HERE, symbolTypes, false);
	}
	else if (direction == SEARCH_UP)
	{
		if (parent())
			return parent()->findSymbols(matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes);
	}

	return {};
}

}
