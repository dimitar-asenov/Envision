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

#include "DependencyUnit.h"

#include "OOModel/src/allOOModelNodes.h"

namespace CppExport {

DependencyUnit::DependencyUnit(QString name, Model::Node* node)
	: name_(name), node_(node), targets_(calculateTargets(node)) {}

QSet<const DependencyUnit*> DependencyUnit::dependencies(const QList<const DependencyUnit*>& allUnits) const
{
	QSet<const DependencyUnit*> result;

	for (auto dependencyTarget : targets_)
	{
		// skip name only dependencies
		if (dependencyTarget.nameOnly_) continue;

		// skip targets pointing into ourself
		if (node_->isAncestorOf(dependencyTarget.target_)) continue;

		for (auto unit : allUnits)
			if (unit != this && unit->node_->isAncestorOf(dependencyTarget.target_))
				result.insert(unit);
	}

	return result;
}

QList<DependencyTarget> DependencyUnit::calculateTargets(Model::Node* node)
{
	QList<DependencyTarget> result;
	for (auto referenceExpression : Model::Node::childrenOfType<OOModel::ReferenceExpression>(node))
		result.append({isNameOnlyDependency(referenceExpression), fixedTarget(referenceExpression)});
	return result;
}

bool DependencyUnit::isNameOnlyDependency(OOModel::ReferenceExpression* reference)
{
	auto parent = reference->parent();
	Q_ASSERT(parent);

	if (reference->firstAncestorOfType<OOModel::MethodCallExpression>()) return false;

	if (DCast<OOModel::TypeQualifierExpression>(parent)) parent = parent->parent();
	if (!DCast<OOModel::PointerTypeExpression>(parent) && !DCast<OOModel::ReferenceTypeExpression>(parent)) return false;

	return true;
}

Model::Node* DependencyUnit::fixedTarget(OOModel::ReferenceExpression* referenceExpression)
{
	if (auto target = referenceExpression->target()) return target;

	// insert custom resolution adjustments here

	return nullptr;
}

}
