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

#include "DiscardConflictingDeletesComponent.h"

#include "ChangeGraph.h"
#include "MergeChange.h"
#include "MergeData.h"

namespace FilePersistence {

void DiscardConflictingDeletesComponent::run(MergeData& mergeData)
{
	auto deletes = deletesConflictingWithMoveOrRelabel(mergeData.cg_);
	computeDeletedSubtrees(mergeData.cg_, deletes);
	removeSuitableDeletes(mergeData.cg_, deletes);
}

QList<MergeChange*> DiscardConflictingDeletesComponent::deletesConflictingWithMoveOrRelabel(ChangeGraph& cg)
{
	QList<MergeChange*> result;

	for (auto change : cg.changesInDirectConflict())
	{
		if (change->type() == ChangeType::Deletion)
		{
			// This is a DELETE change, check:
			// - if it conflicts with exactly one move or one relabel for the same node.
			//   It may conflict with changes of other types, or moves of different nodes
			// - that all conflicts are from the other branch.
			MergeChange* suitableConflictingChange{};
			bool disjointBranches = true;
			for (auto conflictingChange : cg.directConflictsOf(change))
			{
				if (conflictingChange->branches() & change->branches())
				{
					// The branches are not disjoint.
					disjointBranches = false;
					break;
				}

				auto sameId = conflictingChange->nodeId() == change->nodeId();
				auto move = conflictingChange->type() == ChangeType::Move;
				auto relabel = conflictingChange->type() == ChangeType::Stationary
						&& conflictingChange->updateFlags().testFlag(ChangeDescription::Label);

				auto topLevelDelete = true;
				if (relabel)
				{
					for (auto changeThatDependsOnUs : cg.reverseDependenciesOf(change))
						if (changeThatDependsOnUs->type() == ChangeType::Deletion)
						{
							topLevelDelete = false;
							break;
						}
				}

				if (sameId && (move || (relabel && topLevelDelete)) )
				{
					Q_ASSERT(!suitableConflictingChange);
					suitableConflictingChange = conflictingChange;
				}
			}

			if (suitableConflictingChange && disjointBranches) result.append(change);
		}
	}

	return result;
}

void DiscardConflictingDeletesComponent::computeDeletedSubtrees(ChangeGraph& cg, QList<MergeChange*> deletesToLookAt)
{
	// A delete depends on deletes/move-outs which eliminate all children, but we only admit cases where all children
	// are deleted by one branch and none is moved out by that same branch
	for (auto del : deletesToLookAt)
	{
		Q_ASSERT(del->branches() != (MergeChange::BranchA | MergeChange::BranchB));
		Q_ASSERT(del->branches() != MergeChange::None);
		auto deletingBranch = del->branches().testFlag(MergeChange::BranchA) ? MergeChange::BranchA :MergeChange::BranchB;
		isSubtreeCompletelyDeleted(del, deletingBranch, cg);
	}
}

bool DiscardConflictingDeletesComponent::isSubtreeCompletelyDeleted(MergeChange* deleteChange,
																						  MergeChange::Branch deletingBranch,
																						  ChangeGraph& cg)
{
	Q_ASSERT(deleteChange->type() == ChangeType::Deletion);

	// Check if already computed
	if (subtreeIsDeleted.contains(deleteChange, deletingBranch)) return true;
	if (subtreeIsNotDeleted.contains(deleteChange, deletingBranch)) return false;

	auto bothBranchesDelete = deleteChange->branches() == (MergeChange::BranchA | MergeChange::BranchB);

	// Check dependend changes
	bool subtreeDeleted = true;
	for (auto dependency : cg.dependenciesOf(deleteChange))
	{
		// Check if branches match
		auto dependencyMatchesBranch = dependency->branches() & deletingBranch;
		if (!dependencyMatchesBranch)
		{
			// Ignore dependencies of the other branch
			Q_ASSERT(bothBranchesDelete);
			continue;
		}

		// Check if this depeency is a deletion
		if (dependency->type() != ChangeType::Deletion)
		{
			subtreeDeleted = false;
			break;
		}

		subtreeDeleted = isSubtreeCompletelyDeleted(dependency, deletingBranch, cg);
		parentDelete.insert(dependency, deleteChange);
		if (!subtreeDeleted) break;
	}

	// Record and return the result
	if (subtreeDeleted) subtreeIsDeleted.insert(deleteChange, deletingBranch);
	else subtreeIsNotDeleted.insert(deleteChange, deletingBranch);

	return subtreeDeleted;
}

void DiscardConflictingDeletesComponent::removeSuitableDeletes(ChangeGraph& cg, QList<MergeChange*> deletesToLookAt)
{
	// Collect all changes that may be deleted
	QList<MergeChange*> deleteChangesToRemove;
	for (auto del : deletesToLookAt)
	{
		auto deletingBranch = del->branches().testFlag(MergeChange::BranchA) ? MergeChange::BranchA :MergeChange::BranchB;
		if (subtreeIsDeleted.contains(del, deletingBranch))
			deleteChangesToRemove.append(del);
	}

	// From that list, remove only the root deletes
	for (auto change : deleteChangesToRemove)
		if (!isAncestorDeleteInList(change, deleteChangesToRemove))
			cg.removeDeleteChangesConflictingWithMoveOrRelabel(change);
}

bool DiscardConflictingDeletesComponent::isAncestorDeleteInList(MergeChange* change,
																					 QList<MergeChange*> deletesToLookAt)
{
	auto parent = parentDelete.value(change);
	while (parent)
	{
		if (deletesToLookAt.contains(parent)) return true;
		parent = parentDelete.value(parent);
	}
	return false;
}

}
