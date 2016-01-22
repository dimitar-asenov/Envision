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

#include "IfStatement.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
template class Model::TypedList<OOModel::IfStatement>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(IfStatement)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(IfStatement)

REGISTER_ATTRIBUTE(IfStatement, condition, Expression, false, false, true)
REGISTER_ATTRIBUTE(IfStatement, thenBranch, StatementItemList, false, false, true)
REGISTER_ATTRIBUTE(IfStatement, elseBranch, StatementItemList, false, false, true)

bool IfStatement::findSymbols(QSet<Node*>& result, const Model::SymbolMatcher& matcher, const Model::Node* source,
		FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const
{
	if (direction == SEARCH_UP)
	{
		auto ignore = childToSubnode(source);
		Q_ASSERT(ignore);

		bool found{};

		if (condition() != ignore)
			// Optimize the search by skipping the scope of the source, since we've already searched there
			found = condition()->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;
		// Note that a StatementList (the branches) also implements findSymbols and locally declared variables will be
		// found there.

		if ((exhaustAllScopes || !found) && parent())
			found = parent()->findSymbols(result, matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes) || found;

		return found;
	}
	else return false;
}

}
