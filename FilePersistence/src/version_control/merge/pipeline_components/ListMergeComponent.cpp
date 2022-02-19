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

#include "ListMergeComponent.h"
#include "../MergeData.h"
#include "../SoftConflict.h"

#include "Diff3Parse.h"
#include "../../../simple/GenericNode.h"
#include "../../../simple/GenericTree.h"

namespace FilePersistence {

bool ListMergeComponent::isList(const QString& type)
{
	// TestListType is for MergeTests
	return type.startsWith("TypedListOf") || type.endsWith("List") || type.startsWith("TestListType");
}

bool ListMergeComponent::isUnorderedList(const QString& type)
{
	// TODO: The types below sould be configured by plug-ins, not hardcoded here.
	static QStringList unorderedListTypes{"TypedListOfClass",
													  "TypedListOfMethod",
													  "TypedListOfFormalArgument",
													  "TypedListOfFormalResult",
													  "TypedListOfField",
													  "TypedListOfUsedLibrary"};
	return unorderedListTypes.contains(type);
}

bool ListMergeComponent::isOrderedList(const QString& type)
{
	return !isUnorderedList(type) && isList(type);
}

void ListMergeComponent::run(MergeData& mergeData)
{
	switch (phase_) {
		case Phase::Initial:
		{
			listsToMerge_ = computeListsToMerge(mergeData);
			for (auto listId : listsToMerge_)
			{
				auto map = computeAdjustedIndices(listId, mergeData);
				mergeData.cg_.relabelChildrenUniquely(listId, map, mergeData.treeMerged_.get());
			}

			phase_ = Phase::RelabelComplete;
		} break;
		case Phase::RelabelComplete:
		{
			mergeData.cg_.applyNonConflictingChanges(mergeData.treeMerged_.get());
			removeHoles(mergeData.treeMerged_.get(), mergeData.cg_);

			phase_ = Phase::HolesRemoved;
		} break;
		default:
			Q_ASSERT(false);
			break;
	}
}

QList<Model::NodeIdType> ListMergeComponent::computeListsToMerge(MergeData& mergeData)
{
	QHash<Model::NodeIdType, MergeChange::Branches> listsWithStructureChanges;

	// Add every list whose child structure changes
	for (auto change : mergeData.cg_.changes())
	{
		// Ignore value or type changes
		if (change->isValueOrTypeChange()) continue;

		auto oldParentNode = mergeData.treeMerged_->find(change->oldParentId());
		auto newParentNode = mergeData.treeMerged_->find(change->newParentId());
		if (oldParentNode && isList(oldParentNode->type()))
		{
			 auto branches = listsWithStructureChanges.value(change->oldParentId());
			 listsWithStructureChanges.insert(change->oldParentId(), change->branches() | branches);
		}
		if (newParentNode && isList(newParentNode->type()))
		{
			auto branches = listsWithStructureChanges.value(change->newParentId());
			listsWithStructureChanges.insert(change->newParentId(), change->branches() | branches);
		}
	}

	// Keep only lists where both branches makes changes
	// Note that this also automatically filters lists that have been inserted. If we ever want to process lists
	// only modified by one branch and this code is removed, then some code (like the commented one below) will be
	// needed to filter insertions.
	QList<Model::NodeIdType> listsToMerge;
	for (auto it = listsWithStructureChanges.begin(); it != listsWithStructureChanges.end(); ++it)
		if (it.value() == (MergeChange::BranchA | MergeChange::BranchB))
			listsToMerge.append(it.key());

	// The code below is commented because the piece above already takes care of this
	//
	// Remove Lists which are inserted by any version
	// Note that we can't simply check what changes there are for the lists, since the change that
	// inserted a list might have already been applied.
//	auto listIt = listsToMerge.begin();
//	while (listIt != listsToMerge.end())
//	{
//		if (mergeData.treeBase_->find(*listIt, true, true)) ++listIt;
//		else listIt = listsToMerge.erase(listIt);
//	}

	return listsToMerge;
}

ChangeGraph::IdToLabelMap ListMergeComponent::computeAdjustedIndices(Model::NodeIdType listId, MergeData& mergeData)
{
	auto listNode = mergeData.treeMerged_->find(listId);
	Q_ASSERT(listNode);
	auto isOrderedList = ListMergeComponent::isOrderedList(listNode->type());

	ChangeGraph::IdToLabelMap map;
	auto chunks = listToChunks(listId, mergeData);
	int finalIndexInList = 0;	// Final index of elements in the merged tree such that all labels are unique
	for (auto chunk : chunks)
	{
		if (chunk->stable_)
			for (auto id : chunk->spanBase_)
			{
				map.insert(id, {QString::number(finalIndexInList), MergeChange::None});
				finalIndexInList++;
			}
		else
		{
			QList<IdPosition> idPositions;	// stores the relative positions of elements of unstable chunk
			computeOffsetsInBranch(chunk->spanBase_, chunk->spanA_, idPositions, mergeData.treeBase_, MergeChange::BranchA);
			computeOffsetsInBranch(chunk->spanBase_, chunk->spanB_, idPositions, mergeData.treeBase_, MergeChange::BranchB);

			// Add base elements to the list
			// Only add these elements, if they have not already been deleted or moved out
			for (auto id : chunk->spanBase_)
			{
				auto nodeInMergedTree = mergeData.treeMerged_->find(id);
				if (nodeInMergedTree && nodeInMergedTree->parentId() == listId)
				{
					// This node is still in the merged tree and is still under the same parent, so we append it
					idPositions.append({id, mergeData.treeBase_->find(id)->label().toInt(), 0, MergeChange::None});
				}
			}

			std::sort(idPositions.begin(), idPositions.end());	// sort the list according to the labels

			// Merge all adjacent items for the same ID into one position (lists agree where the item should be)
			for (int i = 1; i < idPositions.size();)
			{
				if (idPositions[i].isSamePositionAndID(idPositions[i-1]))
				{
					idPositions[i-1].branches |= idPositions[i].branches;
					idPositions.removeAt(i);
				}
				else ++i;
			}

			IdPosition previousPosition;
			previousPosition.baseIndex = -2; // -1 is used for elements before 0, and we need an index that doesn't exist.
			for (auto idPosition : idPositions)
			{
				map.insert(idPosition.id, {QString::number(finalIndexInList), idPosition.branches});
				finalIndexInList++;

				// Detect soft-conflicts
				if (isOrderedList && idPosition.isSamePosition(previousPosition))
					mergeData.softConflicts_.append(SoftConflict{"List items moved/inserted at the same position",
															  {previousPosition.id, idPosition.id}});

				previousPosition = idPosition;
			}
		}
	}

	for (auto chunk : chunks) delete chunk;
	return map;
}

QList<Chunk*> ListMergeComponent::listToChunks(Model::NodeIdType listId, MergeData& mergeData)
{
	auto idListBase = nodeListToSortedIdList(mergeData.treeBase_->find(listId, true)->children());

	// Below we an empty list for a branch if that list has been deleted by the branch
	// This allows us to shuffle elements in the list nevertheless, even if its deletion doesn't fully succeed
	// in the merged version.

	auto nodeInBranchA = mergeData.treeA_->find(listId, true, true);
	auto idListA = nodeInBranchA ? nodeListToSortedIdList(nodeInBranchA->children()) : QList<Model::NodeIdType>{};

	auto nodeInBranchB = mergeData.treeB_->find(listId, true, true);
	auto idListB = nodeInBranchB ? nodeListToSortedIdList(nodeInBranchB->children()) : QList<Model::NodeIdType>{};

	return Diff3Parse::computeChunks(idListA, idListB, idListBase);
}


QList<Model::NodeIdType> ListMergeComponent::nodeListToSortedIdList(const QList<GenericNode*>& list)
{
	auto comparator = [](GenericNode* const node1, GenericNode* const node2) -> bool
	{
		return node1->label().toInt() < node2->label().toInt();
	};

	auto localList = list;
	std::sort(localList.begin(), localList.end(), comparator);

	QList<Model::NodeIdType> idList;
	int lastIdx = -1;
	for (auto node : localList)
	{
		auto curIdx = node->label().toInt();
		Q_ASSERT(lastIdx < curIdx);
		lastIdx = curIdx;
		idList.append(node->id());
	}
	return idList;
}

void ListMergeComponent::computeOffsetsInBranch(const QList<Model::NodeIdType> base,
																const QList<Model::NodeIdType> version,	QList<IdPosition>& list,
																std::shared_ptr<GenericTree> treeBase, MergeChange::Branches branch)
{
	int baseIndex = -1;
	int offset = 1;
	auto lcs = Diff3Parse::longestCommonSubsequence(base, version);
	for (auto id : version)
	{
		if (lcs.contains(id))
		{
			// The elements from the base tree are purposefully not added to the result list here.
			// That is done later, once for both branches just after this method is called.
			baseIndex = treeBase->find(id)->label().toInt();
			offset = 1;
		}
		else
			list.append({id, baseIndex, offset++, branch});
	}
}

void ListMergeComponent::removeHoles(GenericTree* tree, ChangeGraph& cg)
{
	for (auto listId : listsToMerge_)
	{
		auto listNode = tree->find(listId);
		if (!listNode)
		{
			// This list has been removed
			continue;
		}

		// Traverse through children labels
		ChangeGraph::IdToLabelMap map;
		int i = 0;
		int label = 0;
		int offset = 1;
		auto idList = nodeListToSortedIdList(listNode->children());
		QHash< QString, QPair<int, int>> labelsToBeUpdated;
		while (i < idList.size())
		{
			auto node = listNode->child(QString::number(label));
			if (node)
			{
				map.insert(idList.at(i), {QString::number(i), MergeChange::Branch::None});
				i++;
				offset = 1;
			}
			else
				labelsToBeUpdated.insert(QString::number(label), qMakePair(i-1, offset++));
			label++;
		}
		// Holes at the end of listy are not added in the lableToBeUpdated

		auto listChanges = cg.changesForChildren(listId);
		for (auto change : listChanges)
		{
			// No need to update if change is MoveOut/Deletion Or Stationary change(Not Label)
			if (change->type() == ChangeType::Deletion ||
				(change->type() == ChangeType::Move && change->oldParentId() == listId) ||
				 (change->type() == ChangeType::Stationary && !change->updateFlags().testFlag(ChangeDescription::Label)))
					continue;

			// Insertion Or MoveIn or LabelChange

			// If change corresponding to the label is not present, then hole must be at the end
			QPair<int, int> fractionalIndex;
			auto fractionalIndexIt = labelsToBeUpdated.constFind(change->newLabel());
			if (fractionalIndexIt == labelsToBeUpdated.constEnd())
				fractionalIndex = qMakePair(idList.size()-1, offset++);
			else
				fractionalIndex = fractionalIndexIt.value();

			auto newLabel = QString::number(fractionalIndex.first) + "." + QString::number(fractionalIndex.second);
			map.insert(change->nodeId(), {newLabel, change->branches()});
		}

		cg.relabelChildrenUniquely(listId, map, tree);
	}
}

void ListMergeComponent::printFinalList(const QList<Model::NodeIdType> lists, GenericTree* tree, ChangeGraph& cg)
{
	qDebug() << "Lists";
	for (auto listId : lists)
	{
		qDebug() << listId.toString();
		for (int i = 0; i < tree->find(listId, true)->children().size(); i++)
		{
			auto node = tree->find(listId, true)->child(QString::number(i));
			qDebug() << i << node->id().toString();
		}
		qDebug() << Qt::endl;
	}
	qDebug() << "Changes Conflcting";
	for (auto change : cg.changes())
	{
		qDebug() << change->nodeId() << ":" << change->oldLabel() << "-->" << change->newLabel();
	}
}

void ListMergeComponent::printLabelMap(ChangeGraph::IdToLabelMap map)
{
	for (auto id : map.uniqueKeys())
	{
		qDebug() << id.toString();
		auto i = map.find(id);
		while (i != map.end() && i.key() == id) {
			qDebug() << i.value().label_;
			++i;
		}
	}
}

}
