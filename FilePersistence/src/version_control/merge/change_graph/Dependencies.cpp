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

#include "Dependencies.h"

#include "MergeChange.h"
#include "Conflicts.h"
#include "../../../simple/GenericTree.h"

namespace FilePersistence {

Dependencies::Dependencies(const QList<MergeChange*>& changes,
	const QMultiHash<Model::NodeIdType, MergeChange*>& changesForNode,
	const QMultiHash<Model::NodeIdType, MergeChange*>& changesForChildren)
	: changes_{changes}, changesForNode_{changesForNode}, changesForChildren_{changesForChildren}
{}

void Dependencies::removeAll(MergeChange* change)
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

void Dependencies::copyDependencies(MergeChange* sourceChange, MergeChange* destinationChange)
{
	auto changesSourceDependsOn = dependencies_.values(sourceChange);
	auto changesDependingOnSource = reverseDependencies_.values(sourceChange);

	// All changes that the source depends on, the bdestination must also depend on
	for (auto sourceDependency : changesSourceDependsOn) insert(destinationChange, sourceDependency);

	// All changes that depend on the source, must also depend on the destination
	for (auto dependencyOnSource : changesDependingOnSource) insert(dependencyOnSource, destinationChange);
}

void Dependencies::recomputeAll(GenericTree* tree)
{
	dependencies_.clear();
	reverseDependencies_.clear();

	for (auto change : changes_)
		addDependencies(change, tree);
}


void Dependencies::addDependencies(MergeChange* change, GenericTree* tree)
{
	addParentPresentDependency(change, tree);
	addChildrenRemovedDependency(change, tree);
	addLabelDependency(change);
	addMoveDependency(change, tree);
}


void Dependencies::addParentPresentDependency(MergeChange* change, GenericTree* tree)
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
					insert(change, it.value());
					parentInsertionChangeFound = true;
				}
				++it;
			}
			Q_ASSERT(parentInsertionChangeFound);
		}
	}
}

void Dependencies::addChildrenRemovedDependency(MergeChange* change, GenericTree* tree)
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
					insert(change, it.value());
					changeFound = true;
				}
				++it;
			}
			Q_ASSERT(changeFound);
		}
	}
}

void Dependencies::addLabelDependency(MergeChange* change)
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
			// In the condition below, keep in mind that a Move change may also change the label. Thus it is not
			// sufficient to just check if a change changes the label, because if it's a move, perhaps only the old
			// labels clash and the new ones are fine. This means that the check for Stationary+Label must be like it is,
			// and not simply Label, like it was originally.
			if (it.value()->oldLabel() == change->newLabel() &&
					(	it.value()->type() == ChangeType::Deletion
						|| (it.value()->type() == ChangeType::Stationary
							 && it.value()->updateFlags().testFlag(ChangeDescription::Label))
						|| (it.value()->type() == ChangeType::Move && it.value()->oldParentId() == change->newParentId())
					)
				 )
			{
				insert(change, it.value());
			}
			++it;
		}
	}
}

void Dependencies::addMoveDependency(MergeChange* change, GenericTree* tree)
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
				insert(change, it.value());
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

bool Dependencies::removeDependenciesForSafeMoveChanges(const Conflicts& directConflicts)
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
		if (change->type() == ChangeType::Move && !directConflicts.hasConflicts(change))
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
					remove(change, ourDependency);

					// If other changes depend on this one for cyclicity reasons (not because of label)
					// then make them depend on our dependency
					auto otherDependOnThis = reverseDependencies_.values(change);
					for (auto dependencyToUs : otherDependOnThis)
					{
						if (dependencyToUs->type() == ChangeType::Move
							 && dependencyToUs->newParentId() != change->oldParentId())
						{
							remove(dependencyToUs, change);
							insert(dependencyToUs, ourDependency);
						}
					}
				}
			}
		}
	}

	return removedSomeDependencies;
}

bool Dependencies::removeDependenciesInsideNonConflictingAtomicCycles(const Conflicts& directConflicts)
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
		if ( directConflicts.hasConflicts(change) )
		{
			okToBreakCycle.insert(change, false);
			continue;
		}

		// Check same branches and that dependencies exist
		bool differentBranches = false;
		bool hasDependencies = false;
		for (auto dep : dependencies_.values(change))
		{
			hasDependencies = true;
			if (dep->branches() != change->branches())
			{
				differentBranches = true;
				break;
			}
		}
		if (differentBranches || !hasDependencies)
		{
			okToBreakCycle.insert(change, false);
			continue;
		}

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

								remove(changeToMakeIndependent, dep);
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

}
