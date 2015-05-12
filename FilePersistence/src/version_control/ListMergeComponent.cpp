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

#include "ListMergeComponent.h"
#include "ConflictPairs.h"

namespace FilePersistence {

ListMergeComponent::ListMergeComponent(QSet<QString>& conflictTypes, QSet<QString>& listTypes)
{
	conflictTypes_ = QSet<QString>(conflictTypes);
	listTypes_ = QSet<QString>(listTypes);
}

ListMergeComponent::~ListMergeComponent() {}

void ListMergeComponent::run(const std::unique_ptr<GenericTree>&,
								const std::unique_ptr<GenericTree>&,
								const std::unique_ptr<GenericTree>&,
								ChangeDependencyGraph& cdgA,
								ChangeDependencyGraph& cdgB,
								QSet<ChangeDescription*>& conflictingChanges,
								ConflictPairs& conflictPairs)
{
	// Compute lists  to merge
	for (auto change : conflictingChanges)
	{
		if (onlyChildStructure(change) && listTypes_.contains(change->nodeA()->type()))
		{
			// node of list type and one branch only changes child structure
			ConflictPairs::iterator other = conflictPairs.find(change);
			while (other != conflictPairs.end() &&	other.key() == change &&
					 other.value()->id() != change->id()) other++;
			if (other != conflictPairs.end() && onlyChildStructure(other.value()))
			{
				// other branch only changes child structure
				bool allElementsConflictRoots = true;
				for (auto element : change->nodeA()->children()) {
					if (!(allElementsConflictRoots &= conflictTypes_.contains(element->type())))
						break;
				}
				for (auto element : change->nodeB()->children()) {
					if (!(allElementsConflictRoots &= conflictTypes_.contains(element->type())))
						break;
				}
				for (auto element : other.value()->nodeB()->children()) {
					if (!(allElementsConflictRoots &= conflictTypes_.contains(element->type())))
						break;
				}
				if (allElementsConflictRoots)
				{
					// all elements are conflict roots
					listsToMerge_.insert(change->nodeA());
				}
			}
		}
	}

	for (auto node : listsToMerge_)
	{
		QList<Model::NodeIdType> idListA = nodeListToIdList(cdgA.changes().find(node->id()).value()->nodeB()->children());
		QList<Model::NodeIdType> idListB = nodeListToIdList(cdgB.changes().find(node->id()).value()->nodeB()->children());
		QList<Model::NodeIdType> idListBase = nodeListToIdList(node->children());
		QList<Chunk> chunks = computeMergeChunks(idListA, idListB, idListBase, node->id());
		for (auto chunk : chunks)
		{
			if (chunk.stable_)
			{
				// accept chunk
			}
			else
			{
				QList<Model::NodeIdType> mergedChunk;
				for (auto elem : chunk.spanA_)
				{
					Position posA = findPosition(elem, chunk.spanA_, mergedChunk);
					Position posB = findPosition(elem, chunk.spanB_, mergedChunk);
					Position posBase = findPosition(elem, chunk.spanBase_, mergedChunk);
					auto changeA = cdgA.changes().find(elem);
					bool elemInTreeBase = !(changeA != cdgA.changes().end() && changeA.value()->type() == ChangeType::Insertion);
					auto changeB = cdgB.changes().find(elem);
					bool elemInTreeB = elemInTreeBase &&
							!(changeB != cdgB.changes().end() && changeB.value()->type() == ChangeType::Deletion);

					if (chunk.spanBase_.contains(elem))
					{
						if (chunk.spanB_.contains(elem))
						{
							if (posA == posB)
							{
								if (!mergedChunk.contains(elem)) insertAfter(elem, posA, mergedChunk);
							}
							else
							{
								if (posA != posBase)
								{
									if (posB == posBase) insertAfter(elem, posA, mergedChunk);
									else break; //conflict
								}
							}
						}
					}
					else // elem not in base chunk
					{
						if (elemInTreeB)
						{
							if (idListB.contains(elem))
							{
								if (chunk.spanB_.contains(elem))
								{
									if (posA == posB)
									{
										if (!mergedChunk.contains(elem)) insertAfter(elem, posA, mergedChunk);
									}
									else break; // conflict
								}
								else break; //conflict
							}
							else
							{
								if (idListBase.contains(elem)) break; //conflict
								else insertAfter(elem, posA, mergedChunk);
							}
						}
						else
						{
							if (elemInTreeBase) break; //conflict
							else insertAfter(elem, posA, mergedChunk);
						}
					}
				}
			}
		}
	}
}

/**
 * Tries to find a unique position for \a elem in \a into that is similar to the position of \a elem in \a from.
 * Returns a Position \a pos where \a pos.valid = true iff such a position could be found and pos.predecessor is the
 * element after which \a elem should be inserted or 0 if elem should be inserted at the beginning.
 * Such a position can be found if the nearest predecessor and successor of \elem in \a from that are common in \a into
 * are next to each other and in order in \a into.
 */
ListMergeComponent::Position ListMergeComponent::findPosition(Model::NodeIdType element,
																				  QList<Model::NodeIdType> from, QList<Model::NodeIdType> into)
{
	// positon in empty list is unique
	if (into.isEmpty()) return Position(true, 0);

	int elemIdx = from.indexOf(element) - 1;
	while (elemIdx >= 0 && !into.contains(from.at(elemIdx))) --elemIdx;
	Model::NodeIdType commonPredecessor = (elemIdx != -1) ? from.at(elemIdx) : 0;
	elemIdx = from.indexOf(element) + 1;
	while (elemIdx < from.size() && !into.contains(from.at(elemIdx))) ++elemIdx;
	Model::NodeIdType commonSuccessor = (elemIdx < from.size()) ? from.at(elemIdx) : 0;
	int commonPredIdx = into.indexOf(commonPredecessor);
	int commonSuccIdx = into.indexOf(commonSuccessor);

	if (commonPredIdx + 1 == commonSuccIdx) return Position(true, commonPredecessor);
	else return Position(false, 0);
}


void ListMergeComponent::insertAfter(Model::NodeIdType elem, Position pos, QList<Model::NodeIdType>& chunk)
{
	if (pos.predecessor_ == 0) chunk.prepend(elem);
	else
	{
		int idx = chunk.indexOf(pos.predecessor_);
		chunk.insert(idx + 1, elem);
	}
}

ListMergeComponent::Chunk::Chunk(bool stable, QList<Model::NodeIdType> idListA, QList<Model::NodeIdType> idListB,
						QList<Model::NodeIdType> idListBase)
	: stable_{stable}, spanA_{idListA}, spanB_{idListB}, spanBase_{idListBase} {}

ListMergeComponent::Position::Position(bool valid, Model::NodeIdType predecessor) :
	valid_{valid}, predecessor_{predecessor} {}

QList<ListMergeComponent::Chunk>& ListMergeComponent::computeMergeChunks(const QList<Model::NodeIdType> idListA,
																								 const QList<Model::NodeIdType> idListB,
																								 const QList<Model::NodeIdType> idListBase,
																								 Model::NodeIdType containerId)
{
	QList<Model::NodeIdType> lcsA = longestCommonSubsequence(idListBase, idListA);
	QList<Model::NodeIdType> lcsB = longestCommonSubsequence(idListBase, idListB);

	QSet<Model::NodeIdType> stableA = QSet<Model::NodeIdType>::fromList(lcsA);
	QSet<Model::NodeIdType> stableB = QSet<Model::NodeIdType>::fromList(lcsB);

	QList<Model::NodeIdType> stableIDs;
	for (Model::NodeIdType id : stableA)
		if (stableB.contains(id)) stableIDs.append(id);

	QList<QList<Model::NodeIdType>> sublistsA = computeSublists(idListA, stableIDs);
	QList<QList<Model::NodeIdType>> sublistsB = computeSublists(idListB, stableIDs);
	QList<QList<Model::NodeIdType>> sublistsBase = computeSublists(idListBase, stableIDs);

	QList<Chunk> chunks;

	QList<QList<Model::NodeIdType>>::const_iterator iterA = sublistsA.constBegin();
	QList<QList<Model::NodeIdType>>::const_iterator iterB = sublistsB.constBegin();
	QList<QList<Model::NodeIdType>>::const_iterator iterBase;

	bool isStable = false;
	for (iterBase = sublistsBase.constBegin(); iterBase != sublistsBase.constEnd(); ++iterBase)
	{
		if (!iterBase->isEmpty() || !iterA->isEmpty() || !iterB->isEmpty())
			chunks.append(Chunk(isStable, *iterA, *iterB, *iterBase));
		isStable = !isStable;
		++iterA;
		++iterB;
	}
	Q_ASSERT(isStable);

	auto iterator = mergedLists_.insert(containerId, chunks);

	return iterator.value();
}

QList<QList<Model::NodeIdType>> ListMergeComponent::computeSublists(const QList<Model::NodeIdType> elementIds,
																						  const QList<Model::NodeIdType> stableIDs)
{
	QList<QList<Model::NodeIdType>> chunks;
	QList<Model::NodeIdType> chunk;

	QList<Model::NodeIdType>::const_iterator stableId = stableIDs.begin();
	for (Model::NodeIdType id : elementIds)
	{
		if (stableId != stableIDs.constEnd() && id == *stableId)
		{
			chunks.append(chunk);
			chunk = QList<Model::NodeIdType>();
			chunk.append(id);
			chunks.append(chunk);
			chunk = QList<Model::NodeIdType>();
			stableId++;
		}
		else
			chunk.append(id);
	}
	chunks.append(chunk);

	Q_ASSERT(chunks.size() == 2*stableIDs.size() + 1);

	return chunks;
}

QList<Model::NodeIdType> ListMergeComponent::longestCommonSubsequence(const QList<Model::NodeIdType> listA,
																			const QList<Model::NodeIdType> listB)
{
	int m = listA.size() + 1;
	int n = listB.size() + 1;

	int** lcsLength = new int*[m];
	for (int i = 0; i < m; ++i)
		lcsLength[i] = new int[n];

	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			lcsLength[i][j] = 0;

	for (int i = 1; i < m; ++i)
		for (int j = 1; j < n; ++j)
			if (listA.at(i-1) == listB.at(j-1))
				lcsLength[i][j] = lcsLength[i-1][j-1] + 1;
			else
				lcsLength[i][j] = std::max(lcsLength[i][j-1], lcsLength[i-1][j]);

	QList<Model::NodeIdType> lcs = backtrackLCS(lcsLength, listA, listB, listA.size(), listB.size());

	for (int i = 0; i < m; ++i)
		delete[] lcsLength[i];

	delete[] lcsLength;

	return lcs;
}

QList<Model::NodeIdType> ListMergeComponent::backtrackLCS(int** data, const QList<Model::NodeIdType> listA,
															const QList<Model::NodeIdType> listB, int posA, int posB)
{
	if (posA == 0 || posB == 0)
		return QList<Model::NodeIdType>();
	else if (listA.at(posA-1) == listB.at(posB-1))
		return backtrackLCS(data, listA, listB, posA-1, posB-1)<<listA.at(posA-1);
	else
		if (data[posA][posB-1] > data[posA-1][posB])
			return backtrackLCS(data, listA, listB, posA, posB-1);
		else
			return backtrackLCS(data, listA, listB, posA-1, posB);
}

QList<Model::NodeIdType> ListMergeComponent::nodeListToIdList(const QList<GenericNode*>& list)
{
	QVector<Model::NodeIdType> idList(list.size());
	for (GenericNode* node : list)
	{
		int index = node->name().toInt();
		Q_ASSERT(index >= 0 && index < list.size());
		idList[index] = node->id();
	}
	return QList<Model::NodeIdType>::fromVector(idList);
}


} /* namespace FilePersistence */
