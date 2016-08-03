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

#include "../Diff.h"
#include "../../simple/GenericTree.h"
#include "../../simple/GenericNode.h"

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

	recomputeAllDependencies(tree);
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

	addDirectConflicts(change);
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


void ChangeGraph::addDirectConflicts(MergeChange* change)
{
	addSameNodeConflict(change);
	addLabelConfict(change);
	addDeleteNonEmptyTreeConflict(change);
	addInsertOrMoveToDeletedConflict(change);
}

void ChangeGraph::addSameNodeConflict(MergeChange* change)
{
	// Identical changes have been merged, so remaining changes to the same node either
	//  - do not conflict if one change is for the Value/Type and the other is for the Label/NonStationary
	//  - conflict otherwise
	auto it = changesForNode_.find(change->nodeId());
	while (it != changesForNode_.end() && it.key() == change->nodeId())
	{
		// Make sure not to mark the change as conflicting with itself.
		// Only changes of the same category (value + type / other) may conflict with each other,
		// except for deletion, which conflicts with all other changes.
		if (change != it.value() &&
				(change->isValueOrTypeChange() == it.value()->isValueOrTypeChange()
				 || change->type() == ChangeType::Deletion || it.value()->type() == ChangeType::Deletion))
		{
			directConflicts_.insert(change, it.value());
			directConflicts_.insert(it.value(), change);
		}
		++it;
	}
}

void ChangeGraph::addLabelConfict(MergeChange* change)
{
	// A change might conflict with another change of the same parent if the labels clash
	// Do not consider deletions and value/type changes
	if (!change->isValueOrTypeChange() && change->type() != ChangeType::Deletion)
	{
		Q_ASSERT(!change->newParentId().isNull());

		auto it = changesForChildren_.find(change->newParentId());
		while (it != changesForChildren_.end() && it.key() == change->newParentId())
		{
			// If two changes happen to move a node to the same parent, ignore those changes, they were
			// already covered by the loop above.
			if (change->nodeId() != it.value()->nodeId() // implies change != it.value()
				 && it.value()->type() != ChangeType::Deletion
				 && change->newLabel() == it.value()->newLabel()
				 && change->newParentId() == it.value()->newParentId())
			{
				directConflicts_.insert(change, it.value());
				directConflicts_.insert(it.value(), change);
			}
			++it;
		}
	}
}

void ChangeGraph::addDeleteNonEmptyTreeConflict(MergeChange* change)
{
	// If this change is a deletion, see if some other change moves/inserts a child node, which is a conflict.
	// Changes of existing labels will anyway be in conflict with the deletion of the corresponding nodes. Thus
	// we do not explicitly look for them here.
	if (change->type() == ChangeType::Deletion)
	{
		auto it = changesForChildren_.find(change->nodeId());
		while (it != changesForChildren_.end() && it.key() == change->nodeId())
		{
			if ( (it.value()->type() == ChangeType::Insertion || it.value()->type() == ChangeType::Move)
				  && it.value()->newParentId() == change->nodeId())
			{
				directConflicts_.insert(change, it.value());
				directConflicts_.insert(it.value(), change);
			}
			++it;
		}
	}
}

void ChangeGraph::addInsertOrMoveToDeletedConflict(MergeChange* change)
{
	// If this change inserts or moves a node into a deleted node this is a conflict.
	if (change->type() == ChangeType::Move || change->type() == ChangeType::Insertion)
	{
		Q_ASSERT(!change->newParentId().isNull());
		auto it = changesForNode_.find(change->newParentId());
		while (it != changesForNode_.end() && it.key() == change->newParentId())
		{
			if ( it.value()->type() == ChangeType::Deletion)
			{
				directConflicts_.insert(change, it.value());
				directConflicts_.insert(it.value(), change);
			}
			++it;
		}
	}
}

void ChangeGraph::recomputeAllDependencies(GenericTree* tree)
{
	dependencies_.clear();
	reverseDependencies_.clear();

	for (auto change : changes_)
		addDependencies(change, tree);
}


void ChangeGraph::addDependencies(MergeChange* change, GenericTree* tree)
{
	addParentPresentDependency(change, tree);
	addChildrenRemovedDependency(change, tree);
	addLabelDependency(change);
	addMoveDependency(change, tree);
}


void ChangeGraph::addParentPresentDependency(MergeChange* change, GenericTree* tree)
{
	// Some changes require that the parent node must exist
	// If it is not already in base, find the change that introduces it and depend on it
	if (change->type() == ChangeType::Insertion || change->type() == ChangeType::Move)
	{
		Q_ASSERT(!change->newParentId().isNull());
		bool existsInBase = tree->find(change->newParentId());
		if (!existsInBase)
		{
			// There must be an insertion change introducing this node. Add a dependency on it.
			bool parentInsertionChangeFound = false;
			auto it = changesForNode_.find(change->newParentId());
			while (it != changesForNode_.end() && it.key() == change->newParentId())
			{
				if (it.value()->type() == ChangeType::Insertion)
				{
					dependencies_.insert(change, it.value());
					reverseDependencies_.insert(it.value(), change);
					parentInsertionChangeFound = true;
				}
				++it;
			}
			Q_ASSERT(parentInsertionChangeFound);
		}
	}
}

void ChangeGraph::addChildrenRemovedDependency(MergeChange* change, GenericTree* tree)
{
	// A delete change requires that the node has no children left.
	// Such a change must depend on all changes that delete or move children out.
	if (change->type() == ChangeType::Deletion)
	{
		auto baseNode = tree->find(change->nodeId());
		Q_ASSERT(baseNode);
		for (auto child : baseNode->children())
		{
			bool changeFound = false;
			auto it = changesForNode_.find(child->id());
			while (it != changesForNode_.end() && it.key() == child->id())
			{
				if (it.value()->type() == ChangeType::Deletion
					 || (it.value()->type() == ChangeType::Move && it.value()->oldParentId() == change->nodeId()))
				{
					dependencies_.insert(change, it.value());
					reverseDependencies_.insert(it.value(), change);
					changeFound = true;
				}
				++it;
			}
			Q_ASSERT(changeFound);
		}
	}
}

void ChangeGraph::addLabelDependency(MergeChange* change)
{
	// Labels must be unique. All changes that introduce new labels must depend on other changes
	// that change the existing old label, if any.
	if (change->type() == ChangeType::Insertion || change->type() == ChangeType::Move
		 || change->updateFlags().testFlag(ChangeDescription::Label))
	{
		Q_ASSERT(!change->newParentId().isNull());

		auto it = changesForChildren_.find(change->newParentId());
		while (it != changesForChildren_.end() && it.key() == change->newParentId())
		{
			if (it.value()->oldLabel() == change->newLabel() &&
					(	it.value()->updateFlags().testFlag(ChangeDescription::Label)
						|| it.value()->type() == ChangeType::Deletion
						|| (it.value()->type() == ChangeType::Move && it.value()->oldParentId() == change->newParentId())
					)
				 )
			{
				dependencies_.insert(change, it.value());
				reverseDependencies_.insert(it.value(), change);
			}
			++it;
		}
	}
}


void ChangeGraph::addMoveDependency(MergeChange* change, GenericTree* tree)
{
	// In order to detect cycles each move change depends on the first move change that moves an ancestor of it.
	if (change->type() != ChangeType::Move)
		return;

	// This change might move some tree into a newly inserted tree. Traverse this newly inserted tree first.
	auto ancestorId = change->newParentId();
	bool ancestorIsInserted = true;

	while (ancestorIsInserted)
	{
		ancestorIsInserted = false;
		auto it = changesForNode_.find(ancestorId);
		while (it != changesForNode_.end() && it.key() == ancestorId)
		{
			if (it.value()->type() == ChangeType::Insertion)
			{
				ancestorId = it.value()->newParentId();
				ancestorIsInserted = true;
				break;
			}
			else
				++it;
		}
	}

	// At this point we have an ancestor ID which belongs to a node already in the tree.
	// We should traverse the ancestor chain until we reach the root node or until we reach a moved node
	Q_ASSERT(!ancestorId.isNull());
	while (!ancestorId.isNull())
	{
		bool dependencyFound = false;
		auto it = changesForNode_.find(ancestorId);
		while (it != changesForNode_.end() && it.key() == ancestorId)
		{
			if (it.value()->type() == ChangeType::Move)
			{
				dependencies_.insert(change, it.value());
				reverseDependencies_.insert(it.value(), change);
				dependencyFound = true;
				// Keep looking for additional dependencies, perhaps both branches move the same node to different places.
			}

			++it;
		}

		// If we found the first move, then we're done.
		if (dependencyFound) break;
		else
		{
			auto ancestorNode = tree->find(ancestorId);
			Q_ASSERT(ancestorNode);
			ancestorId = ancestorNode->parentId();
		}
	}
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
			reverseDependencies_.remove(innerIt.value(), outerIt.value());
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
				directConflicts_.remove(innerIt.value(), outerIt.value());
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
		{
			directConflicts_.insert(newChange, otherChange);
			directConflicts_.insert(otherChange, newChange);
		}
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

	// Change A conflicts with Change B
	QList<MergeChange*> changesToConflictWith {change};

	// All of change A's conflicts will also be change B conflicts
	auto conflictIt = directConflicts_.find(change);
	while (conflictIt != directConflicts_.end() && conflictIt.key() == change)
	{
		changesToConflictWith.append(conflictIt.value());
		++conflictIt;
	}
	// Actually add the conflicts
	for (auto & conflict : changesToConflictWith)
	{
		directConflicts_.insert(changeB, conflict);
		directConflicts_.insert(conflict, changeB);
	}

	auto changesADependsOn = dependencies_.values(change);
	auto changesDependingOnA = reverseDependencies_.values(change);

	// All changes that change A depends on, change B must also depend on
	for (auto aDependency : changesADependsOn)
	{
		dependencies_.insert(changeB, aDependency);
		reverseDependencies_.insert(aDependency, changeB);
	}

	// All changes that depend on change A, must also depend on change B
	for (auto dependencyOnA : changesDependingOnA)
	{
		dependencies_.insert(dependencyOnA, changeB);
		reverseDependencies_.insert(changeB, dependencyOnA);
	}
}

void ChangeGraph::applyNonConflictingChanges(GenericTree* tree)
{
	bool tryApplyingMoreChanges = true;
	while (tryApplyingMoreChanges)
	{
		applyIndependentNonConflictingChanges(tree);
		tryApplyingMoreChanges = removeDependenciesForSafeMoveChanges();
		tryApplyingMoreChanges = removeDependenciesInsideNonConflictingAtomicChangeGroups() || tryApplyingMoreChanges;
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
			if (dependencies_.contains(change) || directConflicts_.contains(change))
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

bool ChangeGraph::removeDependenciesForSafeMoveChanges()
{
	// If a move change:
	// - is identical for both branches
	// - depends on exactly one other change, which is also a move change
	// - the dependency is for another node (not because of label clashes)
	// - has no conflicts
	// then we can remove its dependencies and apply it.
	// We know it won't cause a cycle, since both branches make it.
	//
	// Note that any other moves which depend on this one (not because of labels) must be made to depend on the
	// this move's dependency

	bool removedSomeDependencies = false;

	for (auto change : changes_)
	{
		if (change->type() == ChangeType::Move && !directConflicts_.contains(change))
		{
			auto allDependencies = dependencies_.values(change);
			if (allDependencies.size() == 1)
			{
				auto ourDependency = allDependencies.first();
				if (ourDependency->type() == ChangeType::Move && change->newParentId() != ourDependency->oldParentId()
					 && change->branches() == (MergeChange::BranchA | MergeChange::BranchB))
				{
					removedSomeDependencies = true;

					// Remove our dependency
					reverseDependencies_.remove(ourDependency, change);
					dependencies_.remove(change, ourDependency);

					// If other changes depend on this one for cyclicity reasons (not because of label)
					// then make them depend on our dependency
					auto otherDependOnThis = reverseDependencies_.values(change);
					for (auto dependencyToUs : otherDependOnThis)
					{
						if (dependencyToUs->type() == ChangeType::Move
							 && dependencyToUs->newParentId() != change->oldParentId())
						{
							dependencies_.remove(dependencyToUs, change);
							reverseDependencies_.remove(change, dependencyToUs);

							dependencies_.insert(dependencyToUs, ourDependency);
							reverseDependencies_.insert(ourDependency, dependencyToUs);
						}
					}
				}
			}
		}
	}

	return removedSomeDependencies;
}

bool ChangeGraph::removeDependenciesInsideNonConflictingAtomicChangeGroups()
{
	// Atomic change groups are essentially cycles of dependent changes with the following properties:
	// - dependency is due to clashing lables, but other changes, like deletions, moves, and inserts can be in
	//   the cycle. It's not necessary to have stand-alone label changes, e.g. a move in/insert can clash with a
	//   deletion of a node with the same label
	// - all changes come from the same branch. Alternatively both branches should make the same changes.
	//
	// Such changes can be applied, but they need to be applied in an all-or-nothing fashion.
	//
	// To make these non-conflicting and suitable for removal (and subsequent application) the following conditions
	// must also be met
	// - no change depends on a change outside of the cycle. However there might be multiple paths through the cycle.
	// - no change is in conflict with any other change
	//

	bool removedSomeDependencies = false;

	QHash<MergeChange*, bool> okToBreakCycle;
	for (auto change : changes_)
	{
		if (change->isValueOrTypeChange())
		{
			okToBreakCycle.insert(change, false);
			continue;
		}

		// Check no conflicts
		if ( directConflicts_.contains(change) )
		{
			okToBreakCycle.insert(change, false);
			continue;
		}

		// Check same branches
		bool stop = false;
		for (auto dep : dependencies_.values(change))
		{
			if (dep->branches() != change->branches())
			{
				okToBreakCycle.insert(change, false);
				stop = true;
				break;
			}
		}
		if (stop) continue;

		// This change could potentially be removed if the rest of the changes in this cycle are also OK
		okToBreakCycle.insert(change, true);
	}

	//===========================================================================================================
	// For each change, check if it is part of a cycle with all OKs
	for (auto change : changes_)
	{
		if (!okToBreakCycle.value(change)) continue;

		// This change is so far OK. Check if it forms a cycle
		QList<MergeChange*> exploredChanges{change};
		QList<MergeChange*> changesToExplore{dependencies_.values(change)};

		int earliestCycle = -1;
		bool badCycle = false;
		while (!changesToExplore.isEmpty())
		{
			Q_ASSERT(exploredChanges.size() <= changes_.size());

			auto nextChange = changesToExplore.takeLast();

			auto cycleAt = exploredChanges.indexOf(nextChange);
			if (cycleAt >= 0)
			{
				if (earliestCycle < 0 || cycleAt < earliestCycle) earliestCycle = cycleAt;
				continue;
			}

			if (okToBreakCycle.value(nextChange))
			{
				exploredChanges.append(nextChange);
				changesToExplore.append(dependencies_.values(nextChange));
			}
			else
			{
				badCycle = true;
				break;
			}
		}

		if (!badCycle)
		{
			Q_ASSERT(changesToExplore.isEmpty());
			QList<MergeChange*> changesWhoseDependenciesToRemove = exploredChanges.mid(earliestCycle);
			Q_ASSERT( changesWhoseDependenciesToRemove.size() > 1 );

			// This is a cycle which matches all conditions. Remove label clash dependencies between the elements.
			// We ignore label conflicts in this case, since they will be resolved once all changes are applied.
			//
			// Here we still keep non label-clash dependencies, e.g.:
			// -In case of Insertion/MoveIn, we need to make sure that the parent node parent exist
			// -In case of Deletion, we need to make sure that the subtree is Deleted or Moved out

			for (auto changeToMakeIndependent : changesWhoseDependenciesToRemove)
			{
				// Deletion cannot depend on another change because of a label clash, keep all dependencies.
				if (changeToMakeIndependent->type() != ChangeType::Deletion)
				{
					for (auto dep : dependencies_.values(changeToMakeIndependent))
					{
						// An Insertion cannot resolve a label clash, don't remove this dependency
						if (dep->type() != ChangeType::Insertion)
						{
							// changeToMakeIndependent is Move/Insert/Relabel
							// dep is Move/Del/Relabel
							// All but one of the possible dependency combinations are due to label clashes.
							// A move, may depend on a move, not because of a label, but to avoid cycles, so preserve
							// such a depenency
							if (changeToMakeIndependent->type() != ChangeType::Move || dep->type() != ChangeType::Move
								 || changeToMakeIndependent->newParentId() == dep->oldParentId())
							{
								// This must be a label dependency. Remove it.
								Q_ASSERT(changeToMakeIndependent->newLabel() == dep->oldLabel());

								dependencies_.remove(changeToMakeIndependent, dep);
								reverseDependencies_.remove(dep, changeToMakeIndependent);
								removedSomeDependencies = true;
							}
						}
					}
				}
			}
		}

		// Regardless if there was a cycle or not, mark all explored changes as notOK to avoid further processing
		for (auto processedChange : exploredChanges)
			okToBreakCycle.insert(processedChange, false);
	}

	return removedSomeDependencies;
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
	if (mayHaveConflicts)
	{
		QList<MergeChange*> conflicting = directConflicts_.values(change);
		directConflicts_.remove(change);
		for (auto conflict : conflicting) directConflicts_.remove(conflict, change);
	}
	else
		Q_ASSERT(!directConflicts_.contains(change));

	removeAllDependencies(change);

	delete change;
}


QList<MergeChange*>::iterator ChangeGraph::removeChange(QList<MergeChange*>::iterator changeIt, bool mayHaveConflicts)
{
	auto changeToDelete = *changeIt;
	auto returnIt = changes_.erase(changeIt);
	removeChange(changeToDelete, mayHaveConflicts);
	return returnIt;
}

void ChangeGraph::removeAllDependencies(MergeChange* change)
{
	// Remove this change's dependencies
	auto thisDependsOn = dependencies_.values(change);
	auto otherDependOnThis = reverseDependencies_.values(change);
	dependencies_.remove(change);
	reverseDependencies_.remove(change);
	for (auto ourDependency : thisDependsOn)
		reverseDependencies_.remove(ourDependency, change);
	for (auto dependencyToUs : otherDependOnThis)
		dependencies_.remove(dependencyToUs, change);

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
	for (auto conflicting : directConflicts_.values(rootDeleteChange))
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

	// Re-route dependencies on the delete change to now point to the move change
	for (auto dependsOnDel : reverseDependencies_.values(rootDeleteChange))
	{
		// A relabel requires this to be a top-level delete. Make sure that there are no dependencies that are deletes.
		if (relabel) Q_ASSERT(dependsOnDel->type() != ChangeType::Deletion);

		dependencies_.insert(dependsOnDel, suitableConflictingChange);
		reverseDependencies_.insert(suitableConflictingChange, dependsOnDel);
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
			changesToRemove.append( dependencies_.values(change));
			removeChange(change, true);
		}
		else if (change->type() == ChangeType::Deletion)
		{
			// Split a deletion in both branches
			Q_ASSERT(change->branches() == (MergeChange::BranchA | MergeChange::BranchB));
			changesToRemove.append( dependencies_.values(change));
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
