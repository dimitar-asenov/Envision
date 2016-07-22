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

#include "ListMergeComponentV2.h"
#include "MergeData.h"

#include "../Diff3Parse.h"
#include "../../simple/GenericNode.h"
#include "../../simple/GenericTree.h"

namespace FilePersistence {

bool ListMergeComponentV2::isList(const QString& type)
{
	return type.startsWith("TypedListOf") || type.endsWith("List");
}

bool ListMergeComponentV2::isUnorderedList(const QString& type)
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

bool ListMergeComponentV2::isOrderedList(const QString& type)
{
	return !isUnorderedList(type) && isList(type);
}

void ListMergeComponentV2::run(MergeData& mergeData)
{
	mergeData.cg_.applyNonConflictingChanges(mergeData.treeMerged_.get());
	auto listsToMerge = computeListsToMerge(mergeData);
	for (auto listId : listsToMerge)
	{
		auto map = computeAdjustedIndices(listId, mergeData);
		mergeData.cg_.relabelChildrenUniquely(listId, map, mergeData.treeMerged_.get());
		mergeData.cg_.applyNonConflictingChanges(mergeData.treeMerged_.get());

		// Report Conflicts
	}
	removeHoles(listsToMerge, mergeData.treeMerged_.get(), mergeData.cg_);

}

QList<Model::NodeIdType> ListMergeComponentV2::computeListsToMerge(MergeData& mergeData)
{
	QList<Model::NodeIdType> listsToMerge;
	auto changes = mergeData.cg_.changes();
	// Add every list whose child structure changes
	for (auto change : changes)
	{
		// Remove Lists with stationary changes
		if (change->type()==ChangeType::Stationary && !change->updateFlags().testFlag(ChangeDescription::Label))
			continue;
		auto oldParentNode = mergeData.treeMerged_->find(change->oldParentId());
		auto newParentNode = mergeData.treeMerged_->find(change->newParentId());
		if (oldParentNode && isList(oldParentNode->type()) && !listsToMerge.contains(change->oldParentId()))
				listsToMerge.append(change->oldParentId());
		if (newParentNode && isList(newParentNode->type()) && !listsToMerge.contains(change->newParentId()))
				listsToMerge.append(change->newParentId());
	}

	// Remove Lists which are inserted/deleted fully by any version
	auto listIt = listsToMerge.begin();
	while (listIt != listsToMerge.end())
	{
		auto changeList = mergeData.cg_.changesForNode(*listIt);
		bool isInsertionOrDeletion = false;
		for (auto change : changeList)
		{
				if (change->type() == ChangeType::Deletion || change->type() == ChangeType::Insertion)
				{
					isInsertionOrDeletion = true;
					break;
				}
		}
		if (isInsertionOrDeletion)	listIt = listsToMerge.erase(listIt);
		else	++listIt;
	}
	return listsToMerge;
}

ChangeGraph::IdToLabelMap ListMergeComponentV2::computeAdjustedIndices(Model::NodeIdType listId,
																										  MergeData& mergeData)
{
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

			for (auto idPosition : idPositions)
			{
				map.insert(idPosition.id, {QString::number(finalIndexInList), idPosition.branch});
				finalIndexInList++;
			}
			// Report soft-conflict
		}
	}

	for (auto chunk : chunks) delete chunk;
	return map;
}

QList<Chunk*> ListMergeComponentV2::listToChunks(Model::NodeIdType listId, MergeData& mergeData)
{
	auto idListBase = nodeListToSortedIdList(mergeData.treeBase_->find(listId, true)->children());
	auto idListA    = nodeListToSortedIdList(mergeData.treeA_->find(listId, true)->children());
	auto idListB    = nodeListToSortedIdList(mergeData.treeB_->find(listId, true)->children());
	return Diff3Parse::computeChunks(idListA, idListB, idListBase);
}


QList<Model::NodeIdType> ListMergeComponentV2::nodeListToSortedIdList(const QList<GenericNode*>& list)
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

void ListMergeComponentV2::computeOffsetsInBranch(const QList<Model::NodeIdType> base,
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
			baseIndex = treeBase->find(id)->label().toInt();
			offset = 1;
		}
		else
			list.append({id, baseIndex, offset++, branch});
	}
}

void ListMergeComponentV2::removeHoles(const QList<Model::NodeIdType> lists, GenericTree* tree, ChangeGraph& cg)
{
	for (auto listId : lists)
	{
		// Traverse through children labels
		ChangeGraph::IdToLabelMap map;
		int i = 0;
		int label = 0;
		int offset = 1;
		auto listNode = tree->find(listId, true);
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
				fractionalIndex = labelsToBeUpdated.constFind(change->newLabel()).value();

			auto newLabel = QString::number(fractionalIndex.first) + "." + QString::number(fractionalIndex.second);
			map.insert(change->nodeId(), {newLabel, change->branches()});
		}

		cg.relabelChildrenUniquely(listId, map, tree);
	}
}

}
