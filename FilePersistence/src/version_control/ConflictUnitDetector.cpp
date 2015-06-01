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

ConflictUnitDetector::ConflictUnitDetector(QSet<QString>& conflictTypes) : conflictTypes_{conflictTypes} {}

ConflictUnitDetector::~ConflictUnitDetector() {}

RelationAssignmentTransition ConflictUnitDetector::run(const std::unique_ptr<GenericTree>& treeBase,
									const std::unique_ptr<GenericTree>&,
									const std::unique_ptr<GenericTree>&,
									ChangeDependencyGraph& cdgA,
									ChangeDependencyGraph& cdgB,
									QSet<std::shared_ptr<const ChangeDescription>>& conflictingChanges,
									ConflictPairs& conflictPairs, RelationAssignment& oldRelationAssignment)
{
	affectedCUsA_ = computeAffectedCUs(treeBase, cdgA);
	affectedCUsB_ = computeAffectedCUs(treeBase, cdgB);
	RelationAssignmentTransition transition = createIdentityTransition(oldRelationAssignment);
	// In all conflict units...
	for (auto conflictRootId : affectedCUsA_.keys())
	{
		// ...that are modified by both branches...
		if (affectedCUsB_.keys().contains(conflictRootId))
		{
			RelationSet relationSet;
			// ...we take every change from A...
			for (auto changeA : affectedCUsA_.values(conflictRootId))
			{
				// ...mark it as conflicting...
				conflictingChanges.insert(changeA);
				// ...and related to the other changes in this CU
				relationSet->insert(changeA);
				transition.insert(findRelationSet(changeA, oldRelationAssignment), relationSet);
				// ...and take every change from B...
				for (auto changeB : affectedCUsB_.values(conflictRootId))
				{
					// ...mark it conflicting and record the conflict pair.
					conflictingChanges.insert(changeB);
					conflictPairs.insert(changeA, changeB);
					// also record it as being related
					relationSet->insert(changeA);
					transition.insert(findRelationSet(changeB, oldRelationAssignment), relationSet);
				}
			}
		}
		else
		{
			RelationSet relationSet;
			// CU is not in conflict, just record change relations.
			for (auto changeA : affectedCUsA_.values(conflictRootId))
			{
				relationSet->insert(changeA);
				transition.insert(findRelationSet(changeA, oldRelationAssignment), relationSet);
			}
		}
	}
	// also mark changes of same CU as related in B if the CU is not in conflict
	for (auto conflictRootId : affectedCUsB_.keys())
	{
		if (!affectedCUsA_.keys().contains(conflictRootId))
		{
			RelationSet relationSet;
			for (auto changeB : affectedCUsB_.values(conflictRootId))
			{
				relationSet->insert(changeB);
				transition.insert(findRelationSet(changeB, oldRelationAssignment), relationSet);
			}
		}
	}
	return transition;
}

IdToChangeMultiHash ConflictUnitDetector::computeAffectedCUs(const std::unique_ptr<GenericTree>& treeBase,
																				 ChangeDependencyGraph cdg)
{
	IdToChangeMultiHash affectedCUs;
	for (auto change : cdg.changes()) {
		Model::NodeIdType conflictRootA;
		Model::NodeIdType conflictRootB;
		switch (change->type()) {
			case ChangeType::Stationary:
			case ChangeType::Deletion:
				conflictRootA = findConflictUnit(treeBase, change->nodeA());
				affectedCUs.insert(conflictRootA, change);
				break;
			case ChangeType::Insertion:
				conflictRootB = findConflictUnit(treeBase, change->nodeB());
				affectedCUs.insert(conflictRootB, change);
				break;
			case ChangeType::Move:
				conflictRootA = findConflictUnit(treeBase, change->nodeA());
				conflictRootB = findConflictUnit(treeBase, change->nodeB());
				affectedCUs.insert(conflictRootA, change);
				affectedCUs.insert(conflictRootB, change);
				break;
			default:
				Q_ASSERT(false);
		}
	}
	return affectedCUs;
}

Model::NodeIdType ConflictUnitDetector::findConflictUnit(const std::unique_ptr<GenericTree>& treeBase,
											  const GenericNode* node)
{
	// TODO load node, need new functionality from Mitko
	GenericNode* inBase = nullptr; // = find in Base
	while (inBase == nullptr || !node->parentId().isNull())
	{
		node = node->parent(); // TODO load node from HEAD
		inBase = treeBase->find(node);
	}
	if (inBase)
	{
		while (!conflictTypes_.contains(inBase->type())) inBase = inBase->parent(); // TODO load from Base
		return inBase->id();
	}
	else
	{
		// no ancestor in base. branch created new root. return 0.
		return QUuid(0);
	}
}

} /* namespace FilePersistence */
