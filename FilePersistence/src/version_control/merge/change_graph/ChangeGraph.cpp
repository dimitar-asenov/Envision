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

#include "ChangeGraph.h"

#include "../../Diff.h"
#include "../../../simple/GenericTree.h"
#include "../../../simple/GenericNode.h"

namespace FilePersistence {

const QString ChangeGraph::NEW_NODES_PERSISTENT_UNIT_NAME{"ChangeGraphNewNodes"};

ChangeGraph::~ChangeGraph()
{
	for (auto c : changes_) delete c;
	changes_.clear();
}

void ChangeGraph::init(Diff& diffA, Diff& diffB, GenericTree* tree)
{
	QList<MergeChange*> allChanges;

	auto changesA = diffA.changes();
	for (auto it = changesA.begin(); it != changesA.end(); ++it)
		allChanges << MergeChange::changesFromDiffChange(*(it.value()), MergeChange::BranchA);

	auto changesB = diffB.changes();
	for (auto it = changesB.begin(); it != changesB.end(); ++it)
		allChanges <<  MergeChange::changesFromDiffChange(*(it.value()), MergeChange::BranchB);

	insert(allChanges, tree);
}

inline void ChangeGraph::insert(QList<MergeChange*> changes, GenericTree* tree)
{
	for (auto & change : changes) insertSingleChange(change);

	dependencies_.recomputeAll(tree);
}

void ChangeGraph::insertSingleChange(MergeChange* change)
{
	Q_ASSERT(change);

	if (auto existingChange = findIdenticalChange(change))
	{
		existingChange->addBranches(change->branches());
		delete change;

		return;
	}

	changesForNode_.insert(change->nodeId(), change);
	if (!change->oldParentId().isNull())
		changesForChildren_.insert(change->oldParentId(), change);
	if (!change->newParentId().isNull() && change->newParentId() != change->oldParentId())
		changesForChildren_.insert(change->newParentId(), change);
	changes_.append(change);

	directConflicts_.addDirectConflicts(change);
}

MergeChange* ChangeGraph::findIdenticalChange(const MergeChange* change) const
{
	auto it = changesForNode_.find(change->nodeId());
	while (it != changesForNode_.end() && it.key() == change->nodeId())
	{
		if (*it.value() == *change) return it.value();
		++it;
	}

	return nullptr;
}

void ChangeGraph::relabelChildrenUniquely(Model::NodeIdType parentId, IdToLabelMap labelMap, GenericTree* tree)
{
	removeLabelOnlyChangesInChildren(parentId);
	removeLabelDependenciesBetweenChildren(parentId);
	removeLabelConflictsBetweenChildren(parentId);

	updateTreeLabels(parentId, labelMap, tree);	//change the tree directly
	updateLabelsOfChangesTo(parentId, labelMap, tree);
}

void ChangeGraph::removeLabelOnlyChangesInChildren(Model::NodeIdType parentId)
{
	QList<MergeChange*> labelOnlyChanges;
	auto it = changesForChildren_.find(parentId);
	while (it != changesForChildren_.end() && it.key() == parentId)
	{
		if (it.value()->type() == ChangeType::Stationary
			 && it.value()->updateFlags().testFlag(ChangeDescription::Label))
		{
			Q_ASSERT(!it.value()->updateFlags().testFlag(ChangeDescription::Value));
			Q_ASSERT(!it.value()->updateFlags().testFlag(ChangeDescription::Type));

			labelOnlyChanges << it.value();
		}
		++it;
	}

	for (auto changeToRemove : labelOnlyChanges)
		removeChange(changeToRemove, true);
}

void ChangeGraph::removeLabelDependenciesBetweenChildren(Model::NodeIdType parentId)
{
	// Any direct dependencies between children must be label dependencies, so simply remove all of them
	auto outerIt = changesForChildren_.find(parentId);
	while (outerIt != changesForChildren_.end() && outerIt.key() == parentId)
	{
		auto innerIt = changesForChildren_.find(parentId);
		while (innerIt != changesForChildren_.end() && innerIt.key() == parentId)
		{
			dependencies_.remove(outerIt.value(), innerIt.value());
			++innerIt;
		}
		++outerIt;
	}
}

void ChangeGraph::removeLabelConflictsBetweenChildren(Model::NodeIdType parentId)
{
	// Any direct dependencies between children must be label dependencies, so simply remove all of them
	auto outerIt = changesForChildren_.find(parentId);
	while (outerIt != changesForChildren_.end() && outerIt.key() == parentId)
	{
		auto innerIt = changesForChildren_.find(parentId);
		while (innerIt != changesForChildren_.end() && innerIt.key() == parentId)
		{
			if (outerIt.value()->newParentId() == parentId && innerIt.value()->newParentId() == parentId
				 && outerIt.value()->newLabel() == innerIt.value()->newLabel()
				 && outerIt.value()->type() != ChangeType::Deletion
				 && innerIt.value()->type() != ChangeType::Deletion)
			{
				directConflicts_.remove(outerIt.value(), innerIt.value());
			}
			++innerIt;
		}
		++outerIt;
	}
}

void ChangeGraph::updateLabelsOfChangesTo(Model::NodeIdType parentId, IdToLabelMap labelMap, GenericTree* tree)
{
	auto parentNode = tree->find(parentId);
	Q_ASSERT(parentNode);

	auto updateMoveInOrInsert = [this](MergeChange* change, QString& labelA, QString& labelB)
	{
		auto identicalLabels = labelA == labelB;

		// In case both moves/inserts are combined into single move, we split and add two moves.
		// Only split if the two labels are actually different.
		if (change->branches() == (MergeChange::BranchA | MergeChange::BranchB) && !identicalLabels)
		{
			Q_ASSERT(!labelA.isNull() && !labelB.isNull());
			// Update flags are handled in the function
			splitMoveInOrInsertChangeForSecondLabel(change, {labelA, MergeChange::BranchA},
																			{labelB, MergeChange::BranchB});
			labelA.clear();
			labelB.clear();
		}
		else
		{
			// Update labels according to the versions
			if (change->branches().testFlag(MergeChange::BranchA))
			{
				change->newLabel_ = labelA;
				labelA.clear();
			}

			if (change->branches().testFlag(MergeChange::BranchB))
			{
				change->newLabel_ = labelB;
				labelB.clear();
			}
		}
	};

	for (auto nodeId : labelMap.uniqueKeys())
	{
		QString labelA;
		QString labelB;
		QString labelNone;
		auto labels = labelMap.values(nodeId);
		for (auto label : labels)
		{
			if (label.branches_.testFlag(MergeChange::BranchA)) labelA = label.label_;
			if (label.branches_.testFlag(MergeChange::BranchB)) labelB = label.label_;
			if (label.branches_.testFlag(MergeChange::None)) labelNone = label.label_;	//Base
		}

		auto changeIt = changesForNode_.find(nodeId);
		while (changeIt != changesForNode_.end() && changeIt.key() == nodeId)
		{
			switch (changeIt.value()->type())
			{
				case ChangeType::Stationary:
					// Since label changes are already deleted
					Q_ASSERT(changeIt.value()->isValueOrTypeChange() &&
								!changeIt.value()->updateFlags().testFlag(ChangeDescription::Label));
					break;
				case ChangeType::Deletion:	// Update old label in case of deletion
					changeIt.value()->oldLabel_ = labelNone;
					// We don't clear labelNone because it is used as oldLabel for the lblChanges added at the end
					break;
				case ChangeType::Insertion:
				{
					updateMoveInOrInsert(changeIt.value(), labelA, labelB);
					break;
				}

				case ChangeType::Move:
				{
					Q_ASSERT(changeIt.value()->newParentId() == parentId ||
								changeIt.value()->oldParentId() == parentId);
					if (changeIt.value()->newParentId() == parentId)	// Move In
						updateMoveInOrInsert(changeIt.value(), labelA, labelB);
					else	changeIt.value()->oldLabel_ = labelNone;	// Update old Label for Move Out
					// We don't clear labelNone because it is used as oldLabel for the lblChanges added at the end

					// Update Flags
					if (changeIt.value()->newLabel() == changeIt.value()->oldLabel())
						changeIt.value()->updateFlags_ = ChangeDescription::NoFlags;
					else changeIt.value()->updateFlags_ = ChangeDescription::Label;
					break;
				}
				default:
					Q_ASSERT(false);
					break;
			}
			++changeIt;
		}

		// Create additional label changes if labelA or labelB doesn't correspond to any change in CG
		QList<LabelData> newLabels;
		if (!labelA.isNull() && !labelB.isNull() && labelA==labelB)
			newLabels.append({labelA, MergeChange::BranchA | MergeChange::BranchB});
		else
		{
			if (!labelA.isNull()) newLabels.append({labelA, MergeChange::BranchA});
			if (!labelB.isNull()) newLabels.append({labelB, MergeChange::BranchB});
		}
		if (!newLabels.isEmpty())
			createRelabelChanges(nodeId, labelNone, newLabels, parentId);
	}
}

void ChangeGraph::updateTreeLabels(Model::NodeIdType parentId, IdToLabelMap labelMap,	GenericTree* tree)
{
	auto parentNode = tree->find(parentId);

	// Update tree labels according to base Labels in labelMap
	for (auto node : parentNode->children())
	{
		auto labelIt = labelMap.find(node->id());
		while (labelIt != labelMap.end() && labelIt.key() == node->id()) {
			if (labelIt.value().branches_.testFlag(MergeChange::None))	// Base element
			{
				node->setLabel(labelIt.value().label_);
				break;
			}
			++labelIt;
		}
	}
}

void ChangeGraph::createRelabelChanges(Model::NodeIdType nodeId, QString oldLabel, QList<LabelData> newLabels,
													Model::NodeIdType parentId)
{
	QList<MergeChange*> conflictingChangesForThisNode;

	// Get all changes that might conflict with a new relabel change
	auto it = changesForNode_.find(nodeId);
	while (it != changesForNode_.end() && it.key() == nodeId)
	{
		// There should be no insertion
		Q_ASSERT(it.value()->type() != ChangeType::Insertion);
		// There should be no move in
		Q_ASSERT(it.value()->type() != ChangeType::Move || it.value()->oldParentId() == parentId);
		// There should be no existing label changes
		Q_ASSERT(it.value()->type() != ChangeType::Stationary || it.value()->isValueOrTypeChange());

		if (it.value()->type() == ChangeType::Deletion)
			conflictingChangesForThisNode << it.value();
		else if (it.value()->type() == ChangeType::Move)
			conflictingChangesForThisNode << it.value();

		++it;
	}

	// Create new label changes and corresponding conflicts
	for (auto & labelData : newLabels)
	{
		auto newChange = new MergeChange{ChangeType::Stationary, ChangeDescription::Label, nodeId, labelData.branches_,
											parentId, parentId, oldLabel, labelData.label_, {}, {}, {}, {}};

		changes_.append(newChange);
		changesForNode_.insert(nodeId, newChange);
		changesForChildren_.insert(parentId, newChange);

		for (auto otherChange : conflictingChangesForThisNode)
			directConflicts_.insert(newChange, otherChange);
	}
}

void ChangeGraph::splitMoveInOrInsertChangeForSecondLabel(MergeChange* change, LabelData labelOne, LabelData labelTwo)
{
	Q_ASSERT(change->branches() == (MergeChange::BranchA | MergeChange::BranchB) );
	Q_ASSERT(change->type() == ChangeType::Move || change->type() == ChangeType::Insertion);

	// Check that each label is for exactly one branch
	Q_ASSERT(labelOne.branches_ == MergeChange::BranchA || labelOne.branches_ == MergeChange::BranchB);
	Q_ASSERT(labelTwo.branches_ == MergeChange::BranchA || labelTwo.branches_ == MergeChange::BranchB);

	// Adjust the change for the A branch
	change->newLabel_ = labelOne.branches_ == MergeChange::BranchA ? labelOne.label_ : labelTwo.label_;
	if (change->type() == ChangeType::Move)
	{
		if (change->newLabel() == change->oldLabel()) change->updateFlags_ = ChangeDescription::NoFlags;
		else change->updateFlags_ = ChangeDescription::Label;
	}

	if (labelOne.label_ == labelTwo.label_)
	{
		// Both branches move/insert a node and both move/insert it to the same label, which we already updated.
		// Our work here is done
		return;
	}
	else
		change->branches_ = MergeChange::BranchA;

	// Make a new change for label B
	auto newLabelB = labelOne.branches_ == MergeChange::BranchB ? labelOne.label_ : labelTwo.label_;
	auto changeB = new MergeChange{change->type(),
			(( change->type() == ChangeType::Insertion || newLabelB == change->oldLabel())
			 ? ChangeDescription::NoFlags : ChangeDescription::Label),
			change->nodeId(), MergeChange::BranchB,
			change->oldParentId(), change->newParentId(), change->oldLabel(), newLabelB,
			change->oldType(), change->newType(), change->oldValue(), change->newValue()};

	changes_.append(changeB);
	changesForNode_.insert(changeB->nodeId(), changeB);
	changesForChildren_.insert(changeB->oldParentId(), changeB);
	changesForChildren_.insert(changeB->newParentId(), changeB);

	directConflicts_.copyConflicts(change, changeB);
	directConflicts_.insert(change, changeB); // Don't forget to make the new change also conflicting with the original
	dependencies_.copyDependencies(change, changeB);
}

void ChangeGraph::applyNonConflictingChanges(GenericTree* tree)
{
	bool tryApplyingMoreChanges = true;
	while (tryApplyingMoreChanges)
	{
		applyIndependentNonConflictingChanges(tree);
		tryApplyingMoreChanges = dependencies_.removeDependenciesForSafeMoveChanges(directConflicts_);
		tryApplyingMoreChanges = dependencies_.removeDependenciesInsideNonConflictingAtomicCycles(directConflicts_)
											|| tryApplyingMoreChanges;
	}
}

int ChangeGraph::applyIndependentNonConflictingChanges(GenericTree* tree)
{
	int totalAppliedChanges = 0;
	bool appliedSomeChangesThisCycle = true;

	while (appliedSomeChangesThisCycle)
	{
		appliedSomeChangesThisCycle = false;

		auto changeIt = changes_.begin();
		while (changeIt != changes_.end())
		{
			auto change = *changeIt;
			if (dependencies_.hasDependencies(change) || directConflicts_.hasConflicts(change))
			{
				// do nothing
				++changeIt;
			}
			else
			{
				// This change doesn't depend on anything and has no conflicts, apply it
				++totalAppliedChanges;
				appliedSomeChangesThisCycle = true;
				auto change = *changeIt;

				applyChange(tree, change);
				changeIt = removeChange(changeIt, false);
			}
		}
	}

	return totalAppliedChanges;
}

void ChangeGraph::applyChange(GenericTree* tree, MergeChange* change)
{
	switch (change->type())
	{
		case ChangeType::Insertion:
		{
			Q_ASSERT( ! tree->find(change->nodeId()));
			auto persistentUnit = tree->persistentUnit(NEW_NODES_PERSISTENT_UNIT_NAME);
			if (!persistentUnit)
				persistentUnit = &tree->newPersistentUnit(NEW_NODES_PERSISTENT_UNIT_NAME);
			auto node = persistentUnit->newNode(change->nodeId(), change->newParentId(), change->newLabel(),
															change->newType(), change->newValueWithoutPrefix(),
															change->newValueType());
			node->linkNode();
			Q_ASSERT(node->parent()->id() == change->newParentId());
			Q_ASSERT(tree->find(change->nodeId()));
			break;
		}

		case ChangeType::Deletion:
		{
			auto node = tree->find(change->nodeId());
			Q_ASSERT(node);

			Q_ASSERT(node->children().empty());
			tree->remove(change->nodeId());
			Q_ASSERT(!tree->find(change->nodeId()));

			break;
		}

		case ChangeType::Move:
		{
			auto node = tree->find(change->nodeId());
			Q_ASSERT(node);
			Q_ASSERT(node->parentId() == change->oldParentId());

			node->detachFromParent();
			node->setParentId(change->newParentId());
			node->attachToParent();
			Q_ASSERT(node->parent()->id() == change->newParentId());
			// no break, need to do the same stuff as for stationary.
		}

		case ChangeType::Stationary:
		{
			auto node = tree->find(change->nodeId());
			Q_ASSERT(node);

			if (change->updateFlags().testFlag(ChangeDescription::Value))
				node->resetValue(change->newValueType(), change->newValueWithoutPrefix());

			if (change->updateFlags().testFlag(ChangeDescription::Type))
				node->setType(change->newType());

			if (change->updateFlags().testFlag(ChangeDescription::Label))
				node->setLabel(change->newLabel());

			break;
		}

		default:
			Q_ASSERT(false);
	}
}

void ChangeGraph::removeChange(MergeChange* change, bool mayHaveConflicts)
{
	changes_.removeOne(change);

	changesForNode_.remove(change->nodeId(), change);
	if (!change->oldParentId().isNull()) changesForChildren_.remove(change->oldParentId(), change);
	if (!change->newParentId().isNull()) changesForChildren_.remove(change->newParentId(), change);

	// Remove all conflicts
	if (mayHaveConflicts) directConflicts_.removeAll(change);
	else Q_ASSERT(!directConflicts_.hasConflicts(change));

	dependencies_.removeAll(change);

	delete change;
}

QList<MergeChange*>::iterator ChangeGraph::removeChange(QList<MergeChange*>::iterator changeIt, bool mayHaveConflicts)
{
	auto changeToDelete = *changeIt;
	auto returnIt = changes_.erase(changeIt);
	removeChange(changeToDelete, mayHaveConflicts);
	return returnIt;
}

void ChangeGraph::removeDeleteChangesConflictingWithMoveOrRelabel(MergeChange* rootDeleteChange)
{
	// Note that this method will not work if it is called more than once for overlapping trees.
	// Once the smaller subtree is removed, there will be a dependency from a Delete change on a move change
	// from different branches and subsequence calls for removing the super tree will fail.
	// This is on purpose to keep things in the ChangeGraph as simple as possible.
	//
	// Filter subtrees on the outside to only call this method with the super tree.

	// Only one branch should make this change
	Q_ASSERT(rootDeleteChange->branches() != (MergeChange::BranchA | MergeChange::BranchB));
	Q_ASSERT(rootDeleteChange->branches() != MergeChange::None);
	auto deletingBranch = rootDeleteChange->branches().testFlag(MergeChange::BranchA)
			? MergeChange::BranchA :MergeChange::BranchB;

	// It should conflict with a move or relabel from the other branch
	MergeChange* suitableConflictingChange{};
	for (auto conflicting : directConflicts_.conflictsOf(rootDeleteChange))
		if (conflicting->branches().testFlag(deletingBranch) == false
			 && conflicting->nodeId() == rootDeleteChange->nodeId()
			 && (conflicting->type() == ChangeType::Move
				  || (conflicting->type() == ChangeType::Stationary
						&& conflicting->updateFlags().testFlag(ChangeDescription::Label)))
			 )
		{
			suitableConflictingChange = conflicting;
			break;
		}
	Q_ASSERT(suitableConflictingChange);

	auto relabel = suitableConflictingChange->type() == ChangeType::Stationary;

	// Re-route dependencies on the delete change to now point to the move/relabel change
	for (auto dependsOnDel : dependencies_.changesDependingOn(rootDeleteChange))
	{
		// A relabel requires this to be a top-level delete. Make sure that there are no dependencies that are deletes.
		if (relabel) Q_ASSERT(dependsOnDel->type() != ChangeType::Deletion);

		dependencies_.insert(dependsOnDel, suitableConflictingChange);
	}

	// Remove this delete change and all its dependent changes
	// In some cases it is necessary to split the change instead
	// Ignore dependencies from the other branch
	QList<MergeChange*> changesToRemove{rootDeleteChange};
	while (!changesToRemove.isEmpty())
	{
		auto change = changesToRemove.takeLast();

		if (change->branches() == deletingBranch)
		{
			// Remove a deletion if only in this branch
			Q_ASSERT(change->type() == ChangeType::Deletion);
			changesToRemove.append( dependencies_.dependenciesOf(change));
			removeChange(change, true);
		}
		else if (change->type() == ChangeType::Deletion)
		{
			// Split a deletion in both branches
			Q_ASSERT(change->branches() == (MergeChange::BranchA | MergeChange::BranchB));
			changesToRemove.append( dependencies_.dependenciesOf(change));
			change->branches_ &= ~deletingBranch;
		}
		else
		{
			// Any other dependency must be only in the other branch
			// Test this and ignore it
			Q_ASSERT(change->branches().testFlag(deletingBranch) == false);
		}
	}
}

}
