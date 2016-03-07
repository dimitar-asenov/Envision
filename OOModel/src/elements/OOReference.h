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

#include "ModelBase/src/nodes/Reference.h"

namespace OOModel {
	class OOReference;
}
extern template class OOMODEL_API Model::TypedList<OOModel::OOReference>;

namespace OOModel {

class MethodCallExpression;
class Method;

class OOMODEL_API OOReference : public Super<Model::Reference>
{

	NODE_DECLARE_STANDARD_METHODS( OOReference )

	public:
		virtual Node* computeTarget() const override;

		static void unresolveOOReferencesAfterSubTree(Model::Node* subTree);

	private:

		virtual void targetChanged(Node* oldTarget) override;

		enum class ReferenceTargetKind {Unknown, Container, Type, Callable, Assignable, Variable};
		ReferenceTargetKind referenceTargetKind() const;

		Model::Node* resolveAmbiguity(QSet<Model::Node*>& candidates) const;

		Model::Node* resolveAmbiguousMethodCall(QSet<Model::Node*>& candidates, MethodCallExpression* callExpression)
			const;
		// The method above consists of the following steps:
		void removeMethodsWithDifferentNumberOfArguments(QSet<Method*>& methods, MethodCallExpression* callExpression)
			const;
		void removeMethodsWithIncompatibleTypeOfArguments(QSet<Method*>& methods, MethodCallExpression* callExpression)
			const;
		void removeOverridenMethods(QSet<Method*>& methods) const;
		void removeLessSpecificMethods(QSet<Method*>& methods) const;

};

}
