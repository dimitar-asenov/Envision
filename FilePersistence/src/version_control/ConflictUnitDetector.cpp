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
#include "Utils.h"

namespace FilePersistence {

ConflictUnitDetector::ConflictUnitDetector(QSet<QString>& conflictTypes, QString revisionIdA,
														 QString revisionIdB, QString revisionIdBase) :
	conflictTypes_{conflictTypes}, revisionIdA_{revisionIdA}, revisionIdB_{revisionIdB}, revisionIdBase_{revisionIdBase} {}

ConflictUnitDetector::~ConflictUnitDetector() {}

RelationAssignmentTransition ConflictUnitDetector::run(ChangeDependencyGraph& cdgA,
									ChangeDependencyGraph& cdgB,
									QSet<std::shared_ptr<const ChangeDescription>>& conflictingChanges,
									ConflictPairs& conflictPairs, RelationAssignment& oldRelationAssignment)
{
	affectedCUsA_ = computeAffectedCUs(revisionIdA_, cdgA);
	affectedCUsB_ = computeAffectedCUs(revisionIdB_, cdgB);
	RelationAssignmentTransition transition(oldRelationAssignment);
	// In all conflict units...
	for (auto conflictRootId : affectedCUsA_.keys())
	{
		// ...that are modified by both branches...
		if (affectedCUsB_.keys().contains(conflictRootId))
		{
			// ...we take every change from A...
			for (auto changeA : affectedCUsA_.values(conflictRootId))
			{
				// ...mark it as conflicting...
				conflictingChanges.insert(changeA);
				// ...and related to the other changes in this CU
				transition.insert(findRelationSet(changeA, oldRelationAssignment), changeA);
				// ...and take every change from B...
				for (auto changeB : affectedCUsB_.values(conflictRootId))
				{
					// ...mark it conflicting and record the conflict pair.
					conflictingChanges.insert(changeB);
					conflictPairs.insert(changeA, changeB);
					// also record it as being related
					transition.insert(findRelationSet(changeB, oldRelationAssignment), changeB);
				}
			}
		}
		else
		{
			// CU is not in conflict, just record change relations.
			for (auto changeA : affectedCUsA_.values(conflictRootId))
			{
				transition.insert(findRelationSet(changeA, oldRelationAssignment), changeA);
			}
		}
	}
	// also mark changes of same CU as related in B if the CU is not in conflict
	for (auto conflictRootId : affectedCUsB_.keys())
	{
		if (!affectedCUsA_.keys().contains(conflictRootId))
		{
			for (auto changeB : affectedCUsB_.values(conflictRootId))
			{
				transition.insert(findRelationSet(changeB, oldRelationAssignment), changeB);
			}
		}
	}
	return transition;
}

IdToChangeMultiHash ConflictUnitDetector::computeAffectedCUs(const QString revision, ChangeDependencyGraph cdg)
{
	IdToChangeMultiHash affectedCUs;
	for (auto change : cdg.changes()) {
		Model::NodeIdType conflictRootA;
		Model::NodeIdType conflictRootB;
		switch (change->type()) {
			case ChangeType::Stationary:
			case ChangeType::Deletion:
				conflictRootA = findConflictUnit(change->nodeA(), revision, cdg);
				affectedCUs.insert(conflictRootA, change);
				break;
			case ChangeType::Insertion:
				conflictRootB = findConflictUnit(change->nodeB(), revision, cdg);
				affectedCUs.insert(conflictRootB, change);
				break;
			case ChangeType::Move:
				conflictRootA = findConflictUnit(change->nodeA(), revision, cdg);
				conflictRootB = findConflictUnit(change->nodeB(), revision, cdg);
				affectedCUs.insert(conflictRootA, change);
				affectedCUs.insert(conflictRootB, change);
				break;
			default:
				Q_ASSERT(false);
		}
	}
	return affectedCUs;
}

Model::NodeIdType ConflictUnitDetector::findConflictUnit(const GenericNode* node, const QString revision,
																			const ChangeDependencyGraph& cdg)
{
	// find closest ancestor of node that exists in base
	const GenericNode* inBase = getInBase(node, cdg);
	while (inBase == nullptr || !node->parentId().isNull())
	{
		node = getParent(node, false, revision, cdg);
		inBase = getInBase(node, cdg);
	}
	if (inBase)
	{
		while (!conflictTypes_.contains(inBase->type())) inBase = getParent(inBase, true, revisionIdBase_, cdg);
		return inBase->id();
	}
	else
	{
		// no ancestor in base. branch created new root. return 0.
		return QUuid(0);
	}
}

const GenericNode* ConflictUnitDetector::getParent(const GenericNode* node, bool base, const QString revision,
																	const ChangeDependencyGraph& cdg)
{
	// this method tries to load the parent node from the changes instead of searching the repo. This is purely for speed.
	const GenericNode* parent;
	if (node->parent()) parent = node->parent();
	else if (cdg.changes().contains(node->parentId()))
	{
		auto change = cdg.changes().value(node->parentId());
		parent = (base) ? change->nodeA() : change->nodeB();
	}
	else
		parent = Utils::loadNode(node->parentId(), revision, node->persistentUnit());
	Q_ASSERT(parent && parent->id() == node->parentId());
	// TODO set node.parent to newly found parent to avoid looking it up again. This is complicated because of const.
	return parent;
}

/**
 * Returns the node as existing in base or \a nullptr if the node doesn't exist in base.
 */
const GenericNode* ConflictUnitDetector::getInBase(const GenericNode* node, const ChangeDependencyGraph& cdg)
{
	if (cdg.changes().contains(node->id()))
		return cdg.changes().value(node->id())->nodeA();
	else
	{
		// node is not changed, thus not inserted, so it must exist in base. load it.
		return Utils::loadNode(node->id(), revisionIdBase_, node->persistentUnit()); // TODO maybe allocate in different unit
	}
}

} /* namespace FilePersistence */
