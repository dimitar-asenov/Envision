/***********************************************************************************************************************
**
** Copyright (c) 2016 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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

#include "ConflictUnitComponent.h"

#include "ListMergeComponentV2.h"
#include "ChangeGraph.h"
#include "MergeChange.h"
#include "MergeData.h"
#include "../../simple/GenericTree.h"
#include "../../simple/GenericNode.h"

namespace FilePersistence {

bool ConflictUnitComponent::isConflictUnitType(const QString& type)
{
	static QStringList debugConflictUnitTypes{"TestConflictType",
															"TestListType",
															"TestUnorderedType",
															"ListElement",
															"project",
															"package",
															"class",
															"fieldList",
															"methodList",
															"field",
															"method",
															"Method",
															"loop",
															"TypedListOfMethod"};

	// TODO: The types below sould be configured by plug-ins, not hardcoded here.
	static QStringList conflictUnitTypes{"Block",
													 "BreakStatement",
													 "CaseStatement",
													 "ContinueStatement",
													 "DeclarationStatement",
													 "ExpressionStatement",
													 "ForEachStatement",
													 "IfStatement",
													 "LoopStatement",
													 "ReturnStatement",
													 "Statement",
													 "SwitchStatement",
													 "TryCatchFinallyStatement",
													 "Class",
													 "Declaration",
													 "ExplicitTemplateInstantiation",
													 "Field",
													 "Method",
													 "Module",
													 "NameImport",
													 "Project",
													 "TypeAlias",
													 "VariableDeclaration",
													 "CommentStatementItem",
													 "CatchClause"};
	return conflictUnitTypes.contains(type) || ListMergeComponentV2::isList(type)
			|| debugConflictUnitTypes.contains(type);
}

void ConflictUnitComponent::run(MergeData& mergeData)
{
	auto affectedConflictUnits = computeAffectedCUs(mergeData);

	return;

	// For all conflict units...
	for (auto conflictRootId : affectedConflictUnits.first.keys())
	{
		// ...that are modified by both branches...
		if (affectedConflictUnits.second.keys().contains(conflictRootId))
		{
			// ... get all ids of nodes that have changed ...
			auto conflictingIds = affectedConflictUnits.first.values(conflictRootId);
			conflictingIds << affectedConflictUnits.second.values(conflictRootId);

			// ... and add a soft conflict
			mergeData.softConflicts_.append(SoftConflict{"Different branches made changes to the same conflict unit",
													  conflictingIds.toSet()});
		}
	}
}

QPair<ConflictUnitComponent::NodesInConflictUnit, ConflictUnitComponent::NodesInConflictUnit>
ConflictUnitComponent::computeAffectedCUs(MergeData& mergeData)
{
	NodesInConflictUnit affectedConflictUnitsInBranchA;
	NodesInConflictUnit affectedConflictUnitsInBranchB;

	auto processChange = [](MergeChange* change, NodesInConflictUnit& affectedConflictUnits,
			GenericTree* baseTree, GenericTree* branchTree)
	{
		Q_ASSERT(change->type() != ChangeType::Unclassified);
		if (change->type() != ChangeType::Insertion)
			affectedConflictUnits.insert(findConflictUnit(baseTree->find(change->nodeId())), change->nodeId());
		if (change->type() != ChangeType::Deletion)
			affectedConflictUnits.insert(findConflictUnit(branchTree->find(change->nodeId())), change->nodeId());

		// TODO optionally consider structural changes here.
		// Keep in mind that structural changes are not the in the CG, so we would need here to manually "infer" them.
	};

	for (auto change : mergeData.cg_.changes())
	{
		Q_ASSERT(change->branches() != MergeChange::None);
		if (change->branches().testFlag(MergeChange::BranchA))
			processChange(change, affectedConflictUnitsInBranchA, mergeData.treeBase_.get(), mergeData.treeA_.get());
		if (change->branches().testFlag(MergeChange::BranchB))
			processChange(change, affectedConflictUnitsInBranchB, mergeData.treeBase_.get(), mergeData.treeB_.get());
	}

	return qMakePair(affectedConflictUnitsInBranchA, affectedConflictUnitsInBranchB);
}

Model::NodeIdType ConflictUnitComponent::findConflictUnit(const GenericNode* node)
{
	Q_ASSERT(node);
	if (node->parentId().isNull())
	{
		// branch created new root.
		return Model::NodeIdType{};
	}

	while (!isConflictUnitType(node->type()) && !node->parentId().isNull())
	{
		Q_ASSERT(!node->parentId().isNull());
		node = node->parent();
	}
	return node->id();
}

}
