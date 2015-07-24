/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "ConflictUnitDetector.h"
#include "ConflictPairs.h"
#include "ChangeDescription.h"

namespace FilePersistence {

ConflictUnitDetector::ConflictUnitDetector(QSet<QString>& conflictTypes, bool useLinkedChanges) :
	conflictTypes_{conflictTypes}, useLinkedChanges_{useLinkedChanges} {}

ConflictUnitDetector::~ConflictUnitDetector() {}

LinkedChangesTransition ConflictUnitDetector::run(std::shared_ptr<GenericTree>&, std::shared_ptr<GenericTree>&,
																  std::shared_ptr<GenericTree>& treeBase, ChangeDependencyGraph& cdgA,
									ChangeDependencyGraph& cdgB,
									QSet<std::shared_ptr<ChangeDescription>>& conflictingChanges,
									ConflictPairs& conflictPairs, LinkedChangesSet& linkedChangesSet)
{
	treeBase_ = treeBase;
	affectedCUsA_ = computeAffectedCUs(cdgA);
	affectedCUsB_ = computeAffectedCUs(cdgB);
	LinkedChangesTransition transition;
	if (useLinkedChanges_)
		transition = LinkedChangesTransition(linkedChangesSet, cdgA, cdgB);

	// In all conflict units...
	for (auto conflictRootId : affectedCUsA_.keys())
	{
		auto representative = affectedCUsA_.value(conflictRootId);
		// ...that are modified by both branches...
		if (affectedCUsB_.keys().contains(conflictRootId))
		{
			// ...we take every change from A...
			for (auto changeA : affectedCUsA_.values(conflictRootId))
			{
				// ...mark it and depending changes as conflicting...
				conflictingChanges.insert(changeA);
				markDependingAsConflicting(conflictingChanges, changeA, cdgA);
				if (useLinkedChanges_)
					transition.insert(changeA->nodeId(), true, representative, true);
			}
			// ...same with every change from B...
			for (auto changeB : affectedCUsB_.values(conflictRootId))
			{
				conflictingChanges.insert(changeB);
				markDependingAsConflicting(conflictingChanges, changeB, cdgB);
				if (useLinkedChanges_)
					transition.insert(changeB->nodeId(), false, representative, true);
			}
			// ...and add the cross product of the CUs to the conflict pairs...
			for (auto changeA : affectedCUsA_.values(conflictRootId))
				for (auto changeB : affectedCUsB_.values(conflictRootId))
					conflictPairs.insert(changeA, changeB);
		}
		else
		{
			// CU is not in conflict, just record change links.
			for (auto changeA : affectedCUsA_.values(conflictRootId))
			{
				if (useLinkedChanges_)
					transition.insert(changeA->nodeId(), true, representative);
			}
		}
	}

	// also mark changes of same CU as related in B if the CU is not in conflict
	for (auto conflictRootId : affectedCUsB_.keys())
		if (!affectedCUsA_.keys().contains(conflictRootId))
		{
			auto representative = affectedCUsB_.value(conflictRootId);
			for (auto changeB : affectedCUsB_.values(conflictRootId))
			{
				if (useLinkedChanges_)
					transition.insert(changeB->nodeId(), false, representative);
			}
		}

	// down-propagation of child-structure conflicts
	for (auto changeA : cdgA.changes())
	{
		if (changeA->hasFlags(ChangeDescription::Structure))
		{
			auto changeB = cdgB.changes().value(changeA->nodeId());
			if (changeB && changeB->hasFlags(ChangeDescription::Structure))
			{
				auto structureChanges = [](ChangeDependencyGraph& cdg, std::shared_ptr<ChangeDescription> parentChange)
						-> QSet<std::shared_ptr<ChangeDescription>>
				{
					QSet<Model::NodeIdType> childrenIds;
					for (auto child : parentChange->nodeA()->children())
						childrenIds.insert(child->id());
					for (auto child : parentChange->nodeB()->children())
						childrenIds.insert(child->id());
					QSet<std::shared_ptr<ChangeDescription>> changes;
					for (auto childId : childrenIds)
					{
						auto change = cdg.changes().value(childId);
						if (change && (change->type() != ChangeType::Stationary || change->hasFlags(ChangeDescription::Label)))
							changes.insert(cdg.changes().value(childId));
					}
					return changes;
				};

				auto structChangesA = structureChanges(cdgA, changeA);
				auto structChangesB = structureChanges(cdgB, changeB);

				for (auto childChangeA : structChangesA)
				{
					conflictingChanges.insert(childChangeA);
					markDependingAsConflicting(conflictingChanges, childChangeA, cdgA);
					if (useLinkedChanges_)
						transition.insert(childChangeA->nodeId(), true, changeA, true);
				}
				for (auto childChangeB : structChangesB)
				{
					conflictingChanges.insert(childChangeB);
					markDependingAsConflicting(conflictingChanges, childChangeB, cdgB);
					if (useLinkedChanges_)
						transition.insert(childChangeB->nodeId(), false, changeA, true);
				}
				for (auto childChangeA : structChangesA)
					for (auto childChangeB : structChangesB)
						conflictPairs.insert(childChangeA, childChangeB);
			}
		}
	}

	return transition;
}

ConflictUnitSet __attribute__((optimize("O0"))) ConflictUnitDetector::computeAffectedCUs(ChangeDependencyGraph cdg)
{
	ConflictUnitSet affectedCUs;
	for (auto change : cdg.changes()) {
		Model::NodeIdType conflictRootA;
		Model::NodeIdType conflictRootB;
		switch (change->type()) {
			case ChangeType::Deletion:
				conflictRootA = findConflictUnit(change->nodeA());
				affectedCUs.insert(conflictRootA, change);
				break;
			case ChangeType::Insertion:
				conflictRootB = findConflictUnit(change->nodeB());
				affectedCUs.insert(conflictRootB, change);
				break;
			case ChangeType::Move:
			case ChangeType::Stationary:
				conflictRootA = findConflictUnit(change->nodeA());
				conflictRootB = findConflictUnit(change->nodeB());
				affectedCUs.insert(conflictRootA, change);
				affectedCUs.insert(conflictRootB, change);
				break;
			default:
				Q_ASSERT(false);
		}
	}
	return affectedCUs;
}
/* #### This is the old method that finds conflict roots only in base. ####
Model::NodeIdType __attribute__((optimize("O0")))
	ConflictUnitDetector::findConflictUnit(std::shared_ptr<ChangeDescription>& change)
{
	// find closest ancestor of node that exists in base
	Q_ASSERT(change->debugHasNodes());
	GenericNode* inBase = change->nodeA();
	GenericNode* node = change->nodeB();
	Q_ASSERT(inBase || node);
	while (inBase == nullptr && !node->parentId().isNull())
	{
		node = node->parent();
		inBase = treeBase_->find(node->id(), true);
	}
	if (inBase)
	{
		while (!conflictTypes_.contains(inBase->type()))
			inBase = inBase->parent();
		return inBase->id();
	}
	else
	{
		// no ancestor in base. branch created new root.
		return Model::NodeIdType();
	}
}
*/
Model::NodeIdType __attribute__((optimize("O0")))
	ConflictUnitDetector::findConflictUnit(const GenericNode* node)
{
	// find closest ancestor of node that exists in base
	Q_ASSERT(node);
	if (node->parentId().isNull())
	{
		// no ancestor in base. branch created new root.
		return Model::NodeIdType();
	}

	while (!conflictTypes_.contains(node->type()) && !node->parentId().isNull())
	{
		Q_ASSERT(!node->parentId().isNull());
		node = node->parent();
	}
	return node->id();
}

void ConflictUnitDetector::markDependingAsConflicting(QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
										  std::shared_ptr<ChangeDescription>& change, ChangeDependencyGraph& cdg)
{
	for (auto depending : cdg.getDependendingChanges(change))
	{
		conflictingChanges.insert(depending);
		markDependingAsConflicting(conflictingChanges, depending, cdg);
	}

	// Handle label dependencies
	if (change->type() == ChangeType::Deletion ||
		 change->type() == ChangeType::Move ||
		 change->hasFlags(ChangeDescription::Label))
	{
		auto parentId = change->nodeA()->parentId();
		auto parentChange = cdg.changes().value(parentId); // must exist because structFlag
		if (parentChange->type() != ChangeType::Deletion)
		{
			auto oldParentInBranchVersion = parentChange->nodeB();
			// child is in the branch tree and has the same label the changed node had in the base tree.
			auto child = oldParentInBranchVersion->child(change->nodeA()->label());
			if (child)
			{
				auto childChange = cdg.changes().value(child->id());
				if (childChange && !conflictingChanges.contains(childChange) && childChange->type() != ChangeType::Deletion)
				{
					Q_ASSERT(childChange->nodeB()->label() == change->nodeA()->label());
					Q_ASSERT(change->nodeA()->parentId() == childChange->nodeB()->parentId());
					conflictingChanges.insert(childChange);
					// TODO also create conflict pairs
					markDependingAsConflicting(conflictingChanges, childChange, cdg);
				}
			}
		}
	}
}

} /* namespace FilePersistence */
