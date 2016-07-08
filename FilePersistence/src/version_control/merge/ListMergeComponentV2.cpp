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
	// applyNonConflictingChanges(mergeData);
	auto listsToMerge = computeListsToMerge(mergeData);
	for (auto listId : listsToMerge)
	{
		auto map = computeAdjustedIndices(listId, mergeData);
		adjustCG(listId, map, mergeData);
		// applyChanges(mergeData);
		// removeHoles(mergeData);
		// Report Conflicts
	}
}

ListMergeComponentV2::IdToIndexMap ListMergeComponentV2::computeAdjustedIndices(Model::NodeIdType listId,
																										  MergeData& mergeData)
{
	IdToIndexMap map;
	auto chunks = listToChunks(listId, mergeData);
	int index = 0;	// Final index of elements in the merged tree such that all labels are unique
	for (auto chunk : chunks)
	{
		if (chunk->stable_)
			for (auto id : chunk->spanBase_)
			{
				map.insert(id, qMakePair(QString::number(index), 0));
				index++;
			}
		else
		{
			IdPositionMap temp;	// temp will store the relative positions of elements of unstable chunk
			fillIndices(chunk->spanBase_, chunk->spanA_, temp, mergeData.treeBase_);
			fillIndices(chunk->spanBase_, chunk->spanB_, temp, mergeData.treeBase_);
			// Sort temp and insert ids to the map
		}
	}
	return map;
}

void ListMergeComponentV2::adjustCG(Model::NodeIdType /*listId*/,
												IdToIndexMap /*map*/, MergeData& /*mergeData*/)
{}

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

void ListMergeComponentV2::fillIndices(const QList<Model::NodeIdType> base, const QList<Model::NodeIdType> version,
													IdPositionMap& vector, std::shared_ptr<GenericTree> treeBase)
{
	int tempIndex = -1;
	int pair = 1;
	auto lcs = Diff3Parse::longestCommonSubsequence(base, version);
	for (auto id : version)
	{
		if (lcs.contains(id))
		{
			tempIndex = treeBase->find(id)->label().toInt();
			vector.append(qMakePair(qMakePair(tempIndex, 0), id));	//Give it index same as base
			pair = 1;
		}
		else
			vector.append(qMakePair(qMakePair(tempIndex, pair++), id));
	}
}

}
