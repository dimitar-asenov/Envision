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

#include "ValueAtReturnVisitor.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/expressions/ReferenceExpression.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/elements/FormalArgument.h"
#include "OOModel/src/typesystem/TypeArgumentBindings.h"
#include "OOInteraction/src/expression_editor/OOExpressionBuilder.h"
#include "OOInteraction/src/expression_editor/CommandDescriptor.h"
#include "ModelBase/src/model/TreeManager.h"

namespace ContractsLibrary {

OOModel::Method* ValueAtReturnVisitor::ensuresMethod_ = nullptr;
OOModel::Method* ValueAtReturnVisitor::valueAtReturnMethod_ = nullptr;

void ValueAtReturnVisitor::setMethods(OOModel::Method* ensuresMethod, OOModel::Method* valueAtReturnMethod)
{
	ensuresMethod_ = ensuresMethod;
	valueAtReturnMethod_ = valueAtReturnMethod;
}

ValueAtReturnVisitor::ValueAtReturnVisitor() : inEnsuresCall_{false}, inValueAtReturnCall_{false}, outReference_{false},
		numWrapped_{0}, numUnwrapped_{0}
{
}

void ValueAtReturnVisitor::init()
{
	addType<OOModel::MethodCallExpression>(visitMethodCall);
	addType<OOModel::ReferenceExpression>(visitReference);
}

Model::Node* ValueAtReturnVisitor::visitChildren(Model::Node* n)
{
	for (auto child : n->children())
	{
		auto newChild = visit(child);
		if (newChild != child)
		{
			auto manager = n->manager();
			manager->beginModification(n, "replace child in visitor");
			n->replaceChild(child, newChild);
			manager->endModification();
		}
	}

	return n;
}

Model::Node* ValueAtReturnVisitor::visitMethodCall(ValueAtReturnVisitor* v, OOModel::MethodCallExpression* call)
{
	if (ensuresMethod_ && call->methodDefinition({}) == ensuresMethod_)
	{
		v->inEnsuresCall_ = true;
		v->visitChildren(call);
		v->inEnsuresCall_ = false;
		return call;
	}

	if (v->inEnsuresCall_ &&
			valueAtReturnMethod_ && call->methodDefinition({}) == valueAtReturnMethod_ && call->arguments()->size() == 1)
	{
		v->inValueAtReturnCall_ = true;
		v->outReference_ = false;
		v->visitChildren(call);
		v->inValueAtReturnCall_ = false;

		if (!v->outReference_)
		{
			// Replace this method call by its arguments
			auto arg = call->arguments()->at(0);
			auto manager = call->manager();
			manager->beginModification(call, "remove valueAtReturn call in visitor");
			call->arguments()->remove(0);
			manager->endModification();
			++v->numUnwrapped_;

			return arg;
		}
		v->outReference_ = false;
		return call;
	}

	return call;
}

Model::Node* ValueAtReturnVisitor::visitReference(ValueAtReturnVisitor* v, OOModel::ReferenceExpression* ref)
{
	if (v->inEnsuresCall_)
	{
		// Determine if this is an out reference
		auto arg = DCast<OOModel::FormalArgument>(ref->target());
		v->outReference_ = arg && arg->direction() == OOModel::FormalArgument::OUT;

		if (v->outReference_ && !v->inValueAtReturnCall_)
		{
			++v->numWrapped_;
			auto retval = OOInteraction::OOExpressionBuilder::getOOExpression(
					"Contract.ValueAtReturn(" + ref->name() + ")");
			return retval;
		}
	}

	return ref;
}

}
