/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "ConflictUnitComp.h"
#include "ConflictPairs.h"

namespace FilePersistence {

ConflictUnitComp::ConflictUnitComp(QSet<QString>& conflictTypes)
{
	conflictTypes_ = QSet<QString>(conflictTypes);
}

void ConflictUnitComp::run(const std::unique_ptr<GenericTree>& treeBase,
									const std::unique_ptr<GenericTree>&,
									const std::unique_ptr<GenericTree>&,
									ChangeDependencyGraph& cdgA,
									ChangeDependencyGraph& cdgB,
									QSet<ChangeDescription*>& conflictingChanges,
									ConflictPairs& conflictPairs)
{
	computeAffectedCUs(affectedCUsA_, treeBase, cdgA);
	computeAffectedCUs(affectedCUsB_, treeBase, cdgB);
	// In all conflict units...
	foreach (Model::NodeIdType conflictRootId, affectedCUsA_.keys())
	{
		// ...that are modified by both branches...
		if (affectedCUsB_.keys().contains(conflictRootId))
		{
			// ...we take every change from A...
			IdToChangeMultiHash::iterator changeItA = affectedCUsA_.find(conflictRootId);
			while (changeItA != affectedCUsA_.end() && changeItA.key() == conflictRootId)
			{
				// ...mark it as conflicting...
				conflictingChanges.insert(changeItA.value());
				// ...and take every change from B...
				IdToChangeMultiHash::iterator changeItB = affectedCUsB_.find(conflictRootId);
				while (changeItB != affectedCUsB_.end() && changeItB.key() == conflictRootId)
				{
					// ...mark it conflictinging and record the conflict pair.
					conflictingChanges.insert(changeItB.value());
					conflictPairs.insert(changeItA.value(), changeItB.value());
				}
			}
		}
	}
}

void ConflictUnitComp::computeAffectedCUs(IdToChangeMultiHash& cuSet,
								const std::unique_ptr<GenericTree>& treeBase,
								ChangeDependencyGraph cdg)
{
	foreach (ChangeDescription* change, cdg.changes()) {
		Model::NodeIdType conflictRootA;
		Model::NodeIdType conflictRootB;
		switch (change->type()) {
			case ChangeType::Stationary:
			case ChangeType::Deletion:
				conflictRootA = findConflictUnit(treeBase, change->nodeA());
				cuSet.insert(conflictRootA, change);
				break;
			case ChangeType::Insertion:
				conflictRootB = findConflictUnit(treeBase, change->nodeB());
				cuSet.insert(conflictRootB, change);
				break;
			case ChangeType::Move:
				conflictRootA = findConflictUnit(treeBase, change->nodeA());
				conflictRootB = findConflictUnit(treeBase, change->nodeB());
				cuSet.insert(conflictRootA, change);
				cuSet.insert(conflictRootB, change);
				break;
			default:
				Q_ASSERT(false);
		}
	}
}

Model::NodeIdType ConflictUnitComp::findConflictUnit(const std::unique_ptr<GenericTree>& treeBase,
											  const GenericNode* node)
{
	GenericNode* inBase = treeBase->find(node);
	// TODO: think about root node
	while (inBase == nullptr)
	{
		node = node->parent();
		inBase = treeBase->find(node);
	}
	while (!conflictTypes_.contains(inBase->type())) inBase = inBase->parent();
	return inBase->id();
}

} /* namespace FilePersistence */
