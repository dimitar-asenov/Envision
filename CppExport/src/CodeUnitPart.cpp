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
#include "OOModel/src/types/ReferenceType.h"

namespace CppExport {

CodeUnitPart::CodeUnitPart(CodeUnit* parent) : parent_{parent} {}

bool CodeUnitPart::isSourceFragmentEmpty() const
{
	if (!fragment_) return true;

	QList<Export::SourceFragment*> workList{fragment_};
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

void CodeUnitPart::setFragment(Export::SourceFragment* sourceFragment)
{
	fragment_ = sourceFragment;
	if (!fragment_) return;

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
			/*
			 * when comparing two pointers of different types the compiler has to know the full structure of the
			 * classes in order to compute comparable pointers. we compute these hard dependencies here.
			 */
			if (auto parentBinaryOperation = DCast<OOModel::BinaryOperation>(reference->parent()))
				if (auto leftType = parentBinaryOperation->left()->type())
				{
					if (auto rightType = parentBinaryOperation->right()->type())
					{
						auto leftPointerType = dynamic_cast<const OOModel::PointerType*>(leftType.get());
						auto rightPointerType = dynamic_cast<const OOModel::PointerType*>(rightType.get());
						if (leftPointerType && rightPointerType)
						{
							auto leftClassType = dynamic_cast<const OOModel::ClassType*>(leftPointerType->baseType());
							auto rightClassType = dynamic_cast<const OOModel::ClassType*>(rightPointerType->baseType());
							if (leftClassType && rightClassType)
								if (leftClassType->classDefinition() != rightClassType->classDefinition())
								{
									hardTargets_.insert(leftClassType->classDefinition());
									hardTargets_.insert(rightClassType->classDefinition());
								}
						}
					}
				}

			if (!parent()->node()->isAncestorOf(target) || !DCast<OOModel::Class>(target))
			{
				assertForcedDependencyNecessary(reference);

				if (isNameOnlyDependency(reference))
					softTargets_.insert(target);
				else
				{
					auto parentMethodCall = DCast<OOModel::MethodCallExpression>(reference->parent());
					auto prefixReference = DCast<OOModel::ReferenceExpression>(reference->prefix());

					if (!parentMethodCall || !prefixReference || prefixReference->typeArguments()->isEmpty())
						hardTargets_.insert(target);

					// member access
					if (parentMethodCall || DCast<OOModel::ReferenceExpression>(reference->parent()))
					{
						std::unique_ptr<OOModel::Type> baseType{};
						if (parentMethodCall && parentMethodCall->callee()->isAncestorOf(reference))
							baseType = parentMethodCall->type();
						else
							baseType = reference->type();

						const OOModel::Type* finalType = baseType.get();
						if (auto pointerType = dynamic_cast<const OOModel::PointerType*>(finalType))
							finalType = pointerType->baseType();
						else if (auto referenceType = dynamic_cast<const OOModel::ReferenceType*>(finalType))
							finalType = referenceType->baseType();

						if (auto classType = dynamic_cast<const OOModel::ClassType*>(finalType))
							hardTargets_.insert(classType->classDefinition());
					}
				}
			}
		}
}

void CodeUnitPart::assertForcedDependencyNecessary(OOModel::ReferenceExpression* reference)
{
	if (auto parentCast = DCast<OOModel::CastExpression>(reference->parent()->parent()))
		if (parentCast->castType() == reference->parent())
			if (auto grandParentCast = DCast<OOModel::CastExpression>(parentCast->parent()))
				if (auto primitveTypeExpression = DCast<OOModel::PrimitiveTypeExpression>(grandParentCast->castType()))
					if (primitveTypeExpression->typeValue() == OOModel::PrimitiveTypeExpression::PrimitiveTypes::VOID)
						if (DCast<OOModel::NullLiteral>(parentCast->expr()) && hardTargets_.contains(reference->target()))
							Q_ASSERT(false && "redundant forced dependency detected");
}

bool CodeUnitPart::isNameOnlyDependency(OOModel::ReferenceExpression* reference)
{
	auto parent = reference->parent();
	Q_ASSERT(parent);
	if (DCast<OOModel::ExplicitTemplateInstantiation>(parent)) return false;
	else if (reference->firstAncestorOfType<OOModel::ExplicitTemplateInstantiation>()) return true;

	if (auto parentCast = reference->firstAncestorOfType<OOModel::CastExpression>())
		if (parentCast->castType()->isAncestorOf(reference)) return false;

	auto parentClass = reference->firstAncestorOfType<OOModel::Class>();
	if (reference->firstAncestorOfType<OOModel::MethodCallExpression>() &&
		 !reference->typeArguments()->isEmpty() && parentClass && !parentClass->typeArguments()->isEmpty())
		return true;

	if (reference->firstAncestorOfType<OOModel::MethodCallExpression>()) return false;

	if (DCast<OOModel::TypeQualifierExpression>(parent)) parent = parent->parent();
	if (!DCast<OOModel::PointerTypeExpression>(parent) && !DCast<OOModel::ReferenceTypeExpression>(parent)) return false;
	if (auto tryCatchFinally = reference->firstAncestorOfType<OOModel::TryCatchFinallyStatement>())
		if (tryCatchFinally->catchClauses()->isAncestorOf(reference)) return false;

	return true;
}

Model::Node* CodeUnitPart::fixedTarget(OOModel::ReferenceExpression* referenceExpression)
{
	if (auto target = referenceExpression->target()) return target;

	// insert custom resolution adjustments here

	return nullptr;
}

void CodeUnitPart::calculateDependencies(QList<CodeUnit*>& allUnits)
{
	dependencies_.clear();
	if (this == parent()->sourcePart())
		dependencies_.insert(parent()->headerPart());

	for (auto target : hardTargets_)
		for (auto unit : allUnits)
			if (unit->headerPart() != this && unit->headerPart()->nameNodes().contains(target))
				dependencies_.insert(unit->headerPart());
}

QSet<CodeUnitPart*> CodeUnitPart::dependenciesWithinFile(QList<CodeUnit*> units)
{
	QSet<CodeUnitPart*> result;
	for (auto referenceNode : referenceNodes_)
		if (!referenceNode->firstAncestorOfType<OOModel::ExplicitTemplateInstantiation>())
			if (auto target = referenceNode->target())
				for (auto unit : units)
					if (unit->sourcePart() != this &&
						 unit->sourcePart()->nameNodes().contains(target))
							result.insert(unit->sourcePart());
	return result;
}

}
