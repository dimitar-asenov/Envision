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

#include "Conflicts.h"

#include "MergeChange.h"

namespace FilePersistence {

Conflicts::Conflicts(const QList<MergeChange*>& changes,
	const QMultiHash<Model::NodeIdType, MergeChange*>& changesForNode,
	const QMultiHash<Model::NodeIdType, MergeChange*>& changesForChildren)
	: changes_{changes}, changesForNode_{changesForNode}, changesForChildren_{changesForChildren}
{}

void Conflicts::removeAll(MergeChange* change)
{
	QList<MergeChange*> conflicting = directConflicts_.values(change);
	directConflicts_.remove(change);
	for (auto conflict : conflicting) directConflicts_.remove(conflict, change);
}

void Conflicts::addDirectConflicts(MergeChange* change)
{
	addSameNodeConflict(change);
	addLabelConfict(change);
	addDeleteNonEmptyTreeConflict(change);
	addInsertOrMoveToDeletedConflict(change);
}

void Conflicts::addSameNodeConflict(MergeChange* change)
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
			insert(change, it.value());
		}
		++it;
	}
}

void Conflicts::addLabelConfict(MergeChange* change)
{
	// A change might conflict with another change of the same parent if the labels clash
	// Do not consider deletions and value/type changes
	if (!change->isValueOrTypeChange() && change->type() != ChangeType::Deletion)
	{
		Q_ASSERT(!change->newParentId().isNull()); // Bad change, or root node ID has changed

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
				insert(change, it.value());
			}
			++it;
		}
	}
}

void Conflicts::addDeleteNonEmptyTreeConflict(MergeChange* change)
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
				insert(change, it.value());
			}
			++it;
		}
	}
}

void Conflicts::addInsertOrMoveToDeletedConflict(MergeChange* change)
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
				insert(change, it.value());
			}
			++it;
		}
	}
}

void Conflicts::copyConflicts(MergeChange* sourceChange, MergeChange* destinationChange)
{
	for (auto conflict : directConflicts_.values(sourceChange))
		insert(destinationChange, conflict);
}

}
