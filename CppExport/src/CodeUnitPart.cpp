/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "CodeUnitPart.h"
#include "CodeUnit.h"

#include "Export/src/tree/CompositeFragment.h"
#include "OOModel/src/allOOModelNodes.h"
#include "OOModel/src/types/ClassType.h"
#include "OOModel/src/types/PointerType.h"

namespace CppExport {

CodeUnitPart::CodeUnitPart(CodeUnit* parent) : parent_{parent} {}

bool CodeUnitPart::isSourceFragmentEmpty() const
{
	if (!sourceFragment_) return true;

	QList<Export::SourceFragment*> workList{sourceFragment_};
	while (!workList.empty())
	{
		auto current = workList.takeLast();
		if (dynamic_cast<Export::TextFragment*>(current))
			return false;
		else if (auto compositeFragment = dynamic_cast<Export::CompositeFragment*>(current))
			workList << compositeFragment->fragments();
	}
	return true;
}

void CodeUnitPart::setSourceFragment(Export::SourceFragment* sourceFragment)
{
	sourceFragment_ = sourceFragment;

	// calculate name and reference nodes
	nameNodes_.clear();
	referenceNodes_.clear();
	QList<Export::SourceFragment*> workStack{sourceFragment};
	while (!workStack.empty())
	{
		auto fragment = workStack.takeLast();

		auto node = fragment->node();
		if (node->definesSymbol())
			nameNodes_.insert(node);
		else if (auto reference = DCast<OOModel::ReferenceExpression>(node))
			referenceNodes_.insert(reference);

		if (auto compositeFragment = dynamic_cast<Export::CompositeFragment*>(fragment))
			workStack << compositeFragment->fragments();
	}

	// calculate targets
	softTargets_.clear();
	hardTargets_.clear();
	for (auto reference : referenceNodes_)
		if (auto target = fixedTarget(reference))
		{
			if (isNameOnlyDependency(reference))
				softTargets_.insert(target);
			else
			{
				hardTargets_.insert(target);

				// member access
				auto parentMethodCall = DCast<OOModel::MethodCallExpression>(reference->parent());
				if (parentMethodCall || DCast<OOModel::ReferenceExpression>(reference->parent()))
				{
					const OOModel::Type* type{};

					if (parentMethodCall && parentMethodCall->callee()->isAncestorOf(reference))
						type = parentMethodCall->type();
					else
						type = reference->type();

					if (auto pointerType = dynamic_cast<const OOModel::PointerType*>(type))
						type = pointerType->baseType();

					if (auto classType = dynamic_cast<const OOModel::ClassType*>(type))
						hardTargets_.insert(classType->classDefinition());
				}
			}
		}
}

bool CodeUnitPart::isNameOnlyDependency(OOModel::ReferenceExpression* reference)
{
	auto parent = reference->parent();
	Q_ASSERT(parent);

	if (reference->firstAncestorOfType<OOModel::MethodCallExpression>()) return false;

	if (DCast<OOModel::TypeQualifierExpression>(parent)) parent = parent->parent();
	if (!DCast<OOModel::PointerTypeExpression>(parent) && !DCast<OOModel::ReferenceTypeExpression>(parent)) return false;

	return true;
}

Model::Node* CodeUnitPart::fixedTarget(OOModel::ReferenceExpression* referenceExpression)
{
	if (auto target = referenceExpression->target()) return target;

	// insert custom resolution adjustments here

	return nullptr;
}

void CodeUnitPart::calculateDependencies(QList<CodeUnitPart*>& allHeaderParts)
{
	dependencies_.clear();
	if (this == parent()->sourcePart())
		dependencies_.insert(parent()->headerPart());

	for (auto target : hardTargets_)
		for (auto headerPart : allHeaderParts)
			if (headerPart != this && headerPart->nameNodes().contains(target))
					dependencies_.insert(headerPart);
}

}
