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
#include "../change_graph/ChangeGraph.h"
#include "../change_graph/MergeChange.h"
#include "../MergeData.h"
#include "../../../simple/GenericTree.h"
#include "../../../simple/GenericNode.h"

namespace FilePersistence {

bool ConflictUnitComponent::ADD_STRUCTURAL_CHANGES = true;

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

	// For all conflict units...
	for (auto conflictRootId : affectedConflictUnits.first.uniqueKeys())
	{
		// ...that are modified by both branches...
		if (affectedConflictUnits.second.contains(conflictRootId))
		{
			// ... get all ids of nodes that have changed ...
			auto conflictingIdsA = affectedConflictUnits.first.values(conflictRootId).toSet();
			auto conflictingIdsB = affectedConflictUnits.second.values(conflictRootId).toSet();

			// ... and add a soft conflict, but only if not all of the nodes are already in a hard conflict
			if (conflictingIdsA != conflictingIdsB)
				mergeData.softConflicts_.append(SoftConflict{"Different branches made changes to the same conflict unit",
														  conflictingIdsA + conflictingIdsB});
		}
	}
}

QPair<ConflictUnitComponent::NodesInConflictUnit, ConflictUnitComponent::NodesInConflictUnit>
ConflictUnitComponent::computeAffectedCUs(MergeData& mergeData)
{
	NodesInConflictUnit affectedConflictUnitsInBranchA;
	NodesInConflictUnit affectedConflictUnitsInBranchB;

	auto processSingleVersion = [](Model::NodeIdType nodeId, Model::NodeIdType parentId,
			NodesInConflictUnit& affectedConflictUnits, GenericTree* tree)
	{
		auto conflictRoot = findConflictUnit(tree->find(nodeId));
		affectedConflictUnits.insert(conflictRoot, nodeId);

		if (ADD_STRUCTURAL_CHANGES)
			// Add structural changes ...
			if (conflictRoot == nodeId)
			{
				// ... only if we're at a conflict unit boundary, otherwise we're already confliciting with conflicts of
				// our parent.
				auto parentConflictRoot = findConflictUnit(tree->find(parentId, true));
				affectedConflictUnits.insert(parentConflictRoot, parentId);
			}
	};

	auto processChange = [processSingleVersion](MergeChange* change, NodesInConflictUnit& affectedConflictUnits,
			GenericTree* baseTree, GenericTree* branchTree)
	{
		Q_ASSERT(change->type() != ChangeType::Unclassified);
		if (change->type() != ChangeType::Insertion)
			processSingleVersion(change->nodeId(), change->oldParentId(), affectedConflictUnits, baseTree);

		if (change->type() != ChangeType::Deletion)
			processSingleVersion(change->nodeId(), change->newParentId(), affectedConflictUnits, branchTree);
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
