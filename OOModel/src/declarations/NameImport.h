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

#pragma once

#include "../oomodel_api.h"

#include "Declaration.h"
#include "../expressions/Expression.h"

#include "ModelBase/src/nodes/Boolean.h"

namespace OOModel { class NameImport; }
extern template class OOMODEL_API Model::TypedList<OOModel::NameImport>;

namespace OOModel {

class OOMODEL_API NameImport : public Super<Declaration>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(NameImport)
	ATTRIBUTE(Expression, importedName, setImportedName)
	ATTRIBUTE_VALUE(Model::Boolean, importAll, setImportAll, bool)

	public:
		NameImport(Expression* importedName, bool importAllChildrenInScope = false);

		virtual bool definesSymbol() const override;

		virtual bool findSymbols(QSet<Node*>& result, const Model::SymbolMatcher& matcher, const Node* source,
				FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const override;

	private:
		Node* target() const;
};

}
