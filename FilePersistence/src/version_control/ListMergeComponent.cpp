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

ListMergeComponent::ListMergeComponent(QSet<QString>& conflictTypes, QSet<QString>& listTypes,
													QSet<QString>& unorderedTypes) :
	conflictTypes_{conflictTypes}, listTypes_{listTypes}, unorderedTypes_{unorderedTypes} {}

ListMergeComponent::~ListMergeComponent() {}

LinkedChangesTransition ListMergeComponent::run(std::shared_ptr<GenericTree> treeA,
																std::shared_ptr<GenericTree> treeB,
																std::shared_ptr<GenericTree> treeBase,
																ChangeDependencyGraph& cdgA,
																ChangeDependencyGraph& cdgB,
																QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
																ConflictPairs& conflictPairs, LinkedChangesSet& linkedChangesSet)
{
	LinkedChangesTransition transition(linkedChangesSet, cdgA, cdgB);

	auto listsToMerge = computeListsToMerge(cdgA, cdgB, conflictingChanges, conflictPairs);

	for (auto listContainerId : listsToMerge)
	{
		auto listContainerBase = treeBase->find(listContainerId);
		auto idListBase = nodeListToSortedIdList(listContainerBase->children());
		auto idListA = nodeListToSortedIdList(treeA->find(listContainerId)->children());
		auto idListB = nodeListToSortedIdList(treeB->find(listContainerId)->children());
		QList<Model::NodeIdType> mergedList;
		if (listTypes_.contains(listContainerBase->type()))
		{
			// list is ordered
			auto chunks = computeChunks(idListA, idListB, idListBase);
			for (auto chunk : chunks)
			{
				if (chunk.stable_)
					mergedList.append(chunk.spanBase_);
				else
				{
					auto mergedChunk = computeMergedChunk(chunk, cdgA, cdgB, idListA, idListB, idListBase);
					if (mergedChunk.noConflicts_) mergedList.append(mergedChunk.chunk_);
					// else mergedChunks.append(chunk.spanBase_); TODO not sure if this is the best thing to do, maybe keep empty
					// flag right changes as unresolved
				}
			}
		}
		else // list is unordered
		{
			mergedList.append(idListA);
			for (auto elem : idListB)
				if (!mergedList.contains(elem))
				{
					auto pos = findPosition(elem, idListB, mergedList);
					if (!pos.valid_) mergedList.append(elem); // if no valid position is found, we just append.
					else if (pos.predecessor_.isNull()) mergedList.prepend(elem);
					else
					{
						int idx = mergedList.indexOf(pos.predecessor_);
						mergedList.insert(idx + 1, elem);
					}
				}
		}

		// assert that each element occurs only once in the merged list.
		for (auto elemId : mergedList) Q_ASSERT(mergedList.indexOf(elemId) == mergedList.lastIndexOf(elemId));

		// TODO update transition instead of overwrite
		transition = translateListIntoChanges(treeA, treeB, treeBase, listContainerId,
														  mergedList, cdgA, cdgB, linkedChangesSet);
	}

	return transition;
}

std::shared_ptr<ChangeDescription> ListMergeComponent::getConflictingChange(
		ConflictPairs& conflictPairs,
		std::shared_ptr<ChangeDescription>& change)
{
	for (auto other : conflictPairs.values(change))
	{
		if (other->nodeId() == change->nodeId())
			return other;
	}
	return nullptr;
}

QPair<std::shared_ptr<ChangeDescription>, std::shared_ptr<ChangeDescription>> ListMergeComponent::findBranches(
		ChangeDependencyGraph& cdgA,
		std::shared_ptr<ChangeDescription> first,
		std::shared_ptr<ChangeDescription> second)
{
	if (cdgA.changes().value(first->nodeId()) == first)
		return {first, second};
	else
		return {second, first};
}

QPair<bool, QSet<Model::NodeIdType>> ListMergeComponent::checkAndGetAllElementIds(
		std::shared_ptr<ChangeDescription> changeA,
		std::shared_ptr<ChangeDescription> changeB)
{
	QSet<Model::NodeIdType> allElementIds;
	// returns true iff all elements are conflict roots
	auto gatherAndCheckElems = [&](const GenericNode* container) -> bool
	{
		for (auto element : container->children())
		{
			if (!conflictTypes_.contains(element->type()))
				 return false;
			else
				 allElementIds.insert(element->id());
		}
		return true;
	};

	bool allElementsConflictRoots = true;
	allElementsConflictRoots &= gatherAndCheckElems(changeA->nodeA());
	allElementsConflictRoots &= gatherAndCheckElems(changeA->nodeB());
	allElementsConflictRoots &= gatherAndCheckElems(changeB->nodeB());
	return {allElementsConflictRoots, allElementIds};
}

bool onlyConflictsOnLabel(ConflictPairs& conflictPairs, QSet<Model::NodeIdType>& allElementIds,
								  ChangeDependencyGraph& cdgA)
{
	for (auto elem : allElementIds)
	{
		auto changeA = cdgA.changes().value(elem);
		if (changeA)
			for (auto changeB : conflictPairs.values(changeA))
			{
				if (!((changeA->onlyLabelChange() && conflictPairs.values(changeB).size() == 1) ||
						(changeB->onlyLabelChange() && conflictPairs.values(changeA).size() == 1)))
					return false;
			}
	}
	return true;
}

QSet<Model::NodeIdType> ListMergeComponent::computeListsToMerge(
		ChangeDependencyGraph& cdgA,
		ChangeDependencyGraph&,
		QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
		ConflictPairs& conflictPairs)
{
	QSet<Model::NodeIdType> listsToMerge;
	for (auto change : conflictingChanges)
	{
		if (listsToMerge.contains(change->nodeId())) continue;

		if (!change->onlyStructureChange()) continue;

		// At this point we know:
		// - One branch only changes child structure

		if (!listTypes_.contains(change->nodeA()->type()) &&
			 !unorderedTypes_.contains(change->nodeA()->type())) continue;

		// - The node is of list type

		auto conflictingChange = getConflictingChange(conflictPairs, change);
		if (conflictingChange && !conflictingChange->onlyStructureChange()) continue;

		Q_ASSERT(change->nodeA() == conflictingChange->nodeA());

		// - change and conflictingChange affect the same node
		// - Both branches only change child structure

		auto pair = findBranches(cdgA, change, conflictingChange);
		auto changeA = pair.first;
		auto changeB = pair.second;

		// - We know what branches the changes are in

		auto pair2 = checkAndGetAllElementIds(changeA, changeB);
		bool allElementsConflictRoots = pair2.first;
		auto allElementIds = pair2.second;

		if (!allElementsConflictRoots) continue;

		// - All elements of the list are conflict roots

		if (!onlyConflictsOnLabel(conflictPairs, allElementIds, cdgA)) continue;

		// - All conflicts are on label changes only.

		listsToMerge.insert(changeA->nodeId());
	}
	return listsToMerge;
}

ListMergeComponent::ChunkMergeResult ListMergeComponent::computeMergedChunk(Chunk chunk,
																									 ChangeDependencyGraph& cdgA,
																									 ChangeDependencyGraph& cdgB,
																									 QList<Model::NodeIdType>& idListA,
																									 QList<Model::NodeIdType>& idListB,
																									 QList<Model::NodeIdType>& idListBase)
{
	QList<Model::NodeIdType> mergedChunk;
	bool valid = true;
	valid &= insertElemsIntoChunk(mergedChunk, idListBase, chunk.spanBase_,
											cdgA, cdgB, idListB, chunk.spanA_, chunk.spanB_);
	if (valid)
		valid &= insertElemsIntoChunk(mergedChunk, idListBase, chunk.spanBase_,
												cdgB, cdgA, idListA, chunk.spanB_, chunk.spanA_);
	return ChunkMergeResult(valid, mergedChunk);
}

bool ListMergeComponent::insertElemsIntoChunk(QList<Model::NodeIdType>& mergedChunk,
															 const QList<Model::NodeIdType>& idListBase,
															 const QList<Model::NodeIdType>& spanBase,
															 const ChangeDependencyGraph& cdgA,
															 const ChangeDependencyGraph& cdgB,
															 const QList<Model::NodeIdType>& idListB,
															 const QList<Model::NodeIdType>& spanA,
															 const QList<Model::NodeIdType>& spanB)
{
	bool conflict = false;
	for (auto elem : spanA)
	{
		Position posA = findPosition(elem, spanA, mergedChunk);
		Position posB = spanB.contains(elem) ? findPosition(elem, spanB, mergedChunk) : Position(false, {});
		Position posBase = spanBase.contains(elem) ? findPosition(elem, spanBase, mergedChunk) : Position(false, {});

		auto changeA = cdgA.changes().find(elem);
		bool elemInTreeBase = !(changeA != cdgA.changes().end() && changeA.value()->type() == ChangeType::Insertion);
		auto changeB = cdgB.changes().find(elem);
		bool elemInTreeB = elemInTreeBase &&
				!(changeB != cdgB.changes().end() && changeB.value()->type() == ChangeType::Deletion);

		/* If true and the positions are valid, we should insert.
		 * This is false if other branch deletes or element has already been inserted by other branch.
		 */
		bool shouldInsert = false;

		if (spanBase.contains(elem))
		{
			if (spanB.contains(elem))
			{
				if (posA == posB)
				{
					if (!mergedChunk.contains(elem)) shouldInsert = true;
				}
				else
				{
					// NOTE Is this correct, considering how the != operator works for unvalid positions?
					if (posA != posBase)
					{
						if (posB == posBase) shouldInsert = true;
						else
						{
							conflict = true;
							break;
						}
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
					if (spanB.contains(elem))
					{
						if (posA == posB)
						{
							if (!mergedChunk.contains(elem)) shouldInsert = true;
						}
						else
						{
							conflict = true;
							break;
						}
					}
					else
					{
						conflict = true;
						break;
					}
				}
				else
				{
					if (idListBase.contains(elem))
					{
						conflict = true;
						break;
					}
					else shouldInsert = true;
				}
			}
			else
			{
				if (elemInTreeBase)
				{
					conflict = true;
					break;
				}
				else shouldInsert = true;
			}
		}
		// End decision tree

		if (shouldInsert)
		{
			if (posA.valid_)
				insertAfter(elem, posA, mergedChunk);
			else
				conflict = true;
		}

		if (conflict) break;
	}
	return !conflict;
}

ListMergeComponent::Position ListMergeComponent::findPosition(const Model::NodeIdType& element,
																				  const QList<Model::NodeIdType>& from, const QList<Model::NodeIdType>& into)
{
	// position in empty list is unique
	if (into.isEmpty()) return Position(true, {});

	int elemIdx = from.indexOf(element) - 1;
	while (elemIdx >= 0 && !into.contains(from.at(elemIdx))) --elemIdx;
	auto commonPredecessor = (elemIdx >= 0) ? from.at(elemIdx) : Model::NodeIdType();
	elemIdx = from.indexOf(element) + 1;
	while (elemIdx < from.size() && !into.contains(from.at(elemIdx))) ++elemIdx;
	auto commonSuccessor = (elemIdx < from.size()) ? from.at(elemIdx) : Model::NodeIdType();
	int commonPredIdx = into.indexOf(commonPredecessor);
	int commonSuccIdx = into.indexOf(commonSuccessor);

	if (commonPredIdx + 1 == commonSuccIdx) return Position(true, commonPredecessor);
	else return Position(false, commonPredecessor);
}


void ListMergeComponent::insertAfter(Model::NodeIdType elem, Position pos, QList<Model::NodeIdType>& chunk)
{
	Q_ASSERT(pos.valid_);
	if (pos.predecessor_.isNull()) chunk.prepend(elem);
	else
	{
		int idx = chunk.indexOf(pos.predecessor_);
		chunk.insert(idx + 1, elem);
	}
}

QList<ListMergeComponent::Chunk> ListMergeComponent::computeChunks(const QList<Model::NodeIdType> idListA,
																								 const QList<Model::NodeIdType> idListB,
																								 const QList<Model::NodeIdType> idListBase)
{
	auto lcsA = longestCommonSubsequence(idListBase, idListA);
	auto lcsB = longestCommonSubsequence(idListBase, idListB);

	auto stableA = QSet<Model::NodeIdType>::fromList(lcsA);
	auto stableB = QSet<Model::NodeIdType>::fromList(lcsB);

	QList<Model::NodeIdType> stableIDs;
	for (auto id : stableA)
		if (stableB.contains(id)) stableIDs.append(id);

	auto sublistsA = computeSublists(idListA, stableIDs);
	auto sublistsB = computeSublists(idListB, stableIDs);
	auto sublistsBase = computeSublists(idListBase, stableIDs);

	QList<Chunk> chunks;

	auto iterA = sublistsA.constBegin();
	auto iterB = sublistsB.constBegin();

	bool isStable = false;
	for (auto iterBase = sublistsBase.constBegin(); iterBase != sublistsBase.constEnd(); ++iterBase)
	{
		if (!iterBase->isEmpty() || !iterA->isEmpty() || !iterB->isEmpty())
			chunks.append(Chunk(isStable, *iterA, *iterB, *iterBase));
		isStable = !isStable;
		++iterA;
		++iterB;
	}
	Q_ASSERT(isStable);

	return chunks;
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

QList<Model::NodeIdType> ListMergeComponent::nodeListToSortedIdList(const QList<GenericNode*>& list)
{
	QVector<Model::NodeIdType> idList(list.size());
	for (GenericNode* node : list)
	{
		int index = node->label().toInt();
		Q_ASSERT(index >= 0 && index < list.size());
		idList[index] = node->id();
	}
	return QList<Model::NodeIdType>::fromVector(idList);
}

LinkedChangesTransition ListMergeComponent::translateListIntoChanges(std::shared_ptr<GenericTree> treeA,
																							std::shared_ptr<GenericTree>,
																							std::shared_ptr<GenericTree> treeBase,
																							Model::NodeIdType,
																							QList<Model::NodeIdType>& mergedList,
																							ChangeDependencyGraph& cdgA,
																							ChangeDependencyGraph& cdgB,
																							LinkedChangesSet& linkedChangesSet)
{
	for (auto elemId : mergedList)
	{
		auto changeA = cdgA.changes().contains(elemId) ? cdgA.changes().value(elemId) : nullptr;
		auto changeB = cdgB.changes().contains(elemId) ? cdgB.changes().value(elemId) : nullptr;
		if (changeA && !changeA->onlyLabelChange())
		{
			// branch A changes node beyond label
			if (changeB) Q_ASSERT(changeB->onlyLabelChange());
			changeA->nodeB()->setLabel(QString(mergedList.indexOf(elemId)));
			changeA->computeFlags();
		}
		else if (changeB && !changeB->onlyLabelChange())
		{
			// branch B changes node beyond label
			if (changeA) Q_ASSERT(changeA->onlyLabelChange());
			changeB->nodeB()->setLabel(QString(mergedList.indexOf(elemId)));
			changeB->computeFlags();
		}
		else if (changeA)
		{
			changeA->nodeB()->setLabel(QString(mergedList.indexOf(elemId)));
			changeA->computeFlags();
		}
		else if (changeB)
		{
			changeB->nodeB()->setLabel(QString(mergedList.indexOf(elemId)));
			changeB->computeFlags();
		}
		else
		{
			// no branch changes node beyond label so we must construct a new change
			auto oldNode = treeBase->find(elemId);
			auto newNode = treeA->find(elemId);
			Q_ASSERT(oldNode);
			Q_ASSERT(newNode);
			newNode->setLabel(QString(mergedList.indexOf(elemId)));
			auto newChange = std::make_shared<ChangeDescription>(oldNode, newNode);
			cdgA.insert(newChange);
			cdgA.recordDependencies(newChange, true);
			cdgA.recordDependencies(newChange, false);
		}
	}
	return LinkedChangesTransition(linkedChangesSet); // TODO fill properly
}

} /* namespace FilePersistence */
