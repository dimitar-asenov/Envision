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

#include "StatementItemList.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
template class Model::TypedList<OOModel::StatementItemList>;

namespace OOModel {

NODE_DEFINE_EMPTY_CONSTRUCTORS(StatementItemList)
NODE_DEFINE_TYPE_REGISTRATION_METHODS(StatementItemList)

bool StatementItemList::findSymbols(QSet<Node*>& result, const Model::SymbolMatcher& matcher, const Model::Node* source,
		FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const
{
	Q_ASSERT(direction != SEARCH_DOWN);

	if (direction == SEARCH_UP)
	{
		bool found{};

		auto sourceIndex = indexToSubnode(source); // Only search in items above the current one
		if (sourceIndex < 0 || sourceIndex > size()) sourceIndex = size();

		auto ignore = childToSubnode(source);
		for (int i = 0; i<sourceIndex; ++i)
			if (at(i) != ignore)
				// Optimize the search by skipping the scope of the source, since we've already searched there
				found = at(i)->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;

		if ((exhaustAllScopes || !found) && parent())
			found = parent()->findSymbols(result, matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes) || found;

		return found;
	}
	else return Super::findSymbols(result, matcher, source, direction, symbolTypes, exhaustAllScopes);
}

}
