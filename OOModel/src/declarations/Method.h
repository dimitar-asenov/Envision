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

#pragma once

#include "../oomodel_api.h"

#include "Declaration.h"

#include "../elements/StatementItem.h"
#include "../elements/FormalArgument.h"
#include "../elements/FormalResult.h"
#include "../elements/FormalTypeArgument.h"
#include "../elements/StatementItemList.h"
#include "../elements/MemberInitializer.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/TypedList.h"
#include "ModelBase/src/nodes/nodeMacros.h"

namespace OOModel {
	class Method;
}
extern template class OOMODEL_API Model::TypedList<OOModel::Method>;

namespace OOModel {

class OOMODEL_API Method : public Super<Declaration>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(Method)

	ATTRIBUTE(StatementItemList, items, setItems)
	ATTRIBUTE(Model::TypedList<FormalTypeArgument>, typeArguments, setTypeArguments)
	ATTRIBUTE(Model::TypedList<FormalArgument>, arguments, setArguments)
	ATTRIBUTE(Model::TypedList<FormalResult>, results, setResults)
	ATTRIBUTE(Model::TypedList<MemberInitializer>, memberInitializers, setMemberInitializers)
	ATTRIBUTE(Model::TypedList<Expression>, throws, setThrows)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, mthKind, setMthKind, int)

	public:
		Method(const QString& name);
		Method(const QString& name, Modifier::Modifiers mod);

		enum class MethodKind : int {Default, Constructor, Destructor, Conversion, OperatorOverload};

		Method(const QString& name, MethodKind kind);
		Method(const QString& name, Modifier::Modifiers mod, MethodKind kind);

		MethodKind methodKind() const;
		void setMethodKind(const MethodKind& kind);

		QString fullyQualifiedName() const;
		bool isGeneric();

		bool isStatic() const;

		virtual SymbolTypes symbolType() const override;

		virtual bool findSymbols(QSet<Node*>& result, const Model::SymbolMatcher& matcher, const Node* source,
				FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const override;

		bool overrides(Method* other);
		bool isOverridenBy(Method* other);

		QSet<Method*> callees();
		QSet<Method*> callers();

		virtual bool isNewPersistenceUnit() const override;
};

inline Method::MethodKind Method::methodKind() const { return static_cast<MethodKind> (mthKind()); }
inline void Method::setMethodKind(const MethodKind &kind) { setMthKind(static_cast<int> (kind)); }
inline bool Method::isStatic() const { return modifiers()->isSet(Modifier::Static); }
inline bool Method::isOverridenBy(Method* other) {return other->overrides(this);}

}
