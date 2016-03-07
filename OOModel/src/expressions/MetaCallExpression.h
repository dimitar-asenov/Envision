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

#include "Expression.h"
#include "ModelBase/src/nodes/TypedList.h"

namespace OOModel {
	class MetaCallExpression;
}
extern template class OOMODEL_API Model::TypedList<OOModel::MetaCallExpression>;

namespace OOModel {

class MetaDefinition;
class Declaration;
class MetaBinding;

class OOMODEL_API MetaCallExpression: public Super<Expression>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(MetaCallExpression)

	ATTRIBUTE(Expression, callee, setCallee)
	ATTRIBUTE(Model::List, arguments, setArguments)
	PRIVATE_ATTRIBUTE(Model::Node, cache, setCache)

	public:
		MetaCallExpression(const QString& name, Expression* referencePrefix = nullptr);

		/**
		 * Returns the meta definition used in this meta call if one exists.
		 */
		MetaDefinition* metaDefinition();

		/**
		 * Returns the generated tree resulting from this meta call.
		 */
		Declaration* generatedTree();

	private:
		/**
		 * Binds all MetaCallExpressions according to the binding in node.
		 */
		void bindMetaCalls(Model::Node* node, MetaBinding* binding);
};

}
