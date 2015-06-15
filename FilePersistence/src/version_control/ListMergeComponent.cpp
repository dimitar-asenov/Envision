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

QSet<Model::NodeIdType> ListMergeComponent::computeListsToMerge(
		ChangeDependencyGraph& cdgA,
		ChangeDependencyGraph& cdgB,
		QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
		ConflictPairs& conflictPairs)
{
	QSet<Model::NodeIdType> listsToMerge;
	for (auto change : conflictingChanges)
	{
		if (!change->onlyStructureChange()) continue;
		if (!listTypes_.contains(change->nodeA()->type()) &&
			 !unorderedTypes_.contains(change->nodeA()->type())) continue;

		// node of list type and one branch only changes child structure
		// look for change of other branch of same node
		std::shared_ptr<ChangeDescription> conflictingChange;
		bool conflictingChangeFound = false;
		for (auto other : conflictPairs.values(change))
		{
			if (other->nodeId() == change->nodeId())
			{
				conflictingChange = other;
				conflictingChangeFound = true;
				break;
			}
		}

		if (conflictingChangeFound && !conflictingChange->onlyStructureChange()) continue;
		// other branch only changes child structure

		// Now we have change on list container and conflicting change on the same node.

		std::shared_ptr<ChangeDescription> changeA;
		std::shared_ptr<ChangeDescription> changeB;
		if (cdgA.changes().value(change->nodeId()) == change)
		{
			changeA = change;
			changeB = conflictingChange;
		}
		else
		{
			changeA = conflictingChange;
			changeB = change;
		}

		Q_ASSERT(changeA->nodeA() == changeB->nodeA()); // both changes point to the same base node object.

		QSet<Model::NodeIdType> allElementIds;
		// returns true iff all elements are conflict roots
		auto gatherAndCheckElems = [&](const GenericNode* container) -> bool
		{
			bool allElementsConflictRoots = true;
			for (auto element : container->children())
			{
				allElementIds.insert(element->id());
				allElementsConflictRoots &= conflictTypes_.contains(element->type());
				if (!allElementsConflictRoots) break;
			}
			return allElementsConflictRoots;
		};

		bool allElementsConflictRoots = true;
		allElementsConflictRoots &= gatherAndCheckElems(changeA->nodeA());
		allElementsConflictRoots &= gatherAndCheckElems(changeA->nodeB());
		allElementsConflictRoots &= gatherAndCheckElems(changeB->nodeB());

		if (!allElementsConflictRoots) continue;

		// for all elements, check that conflicts are on label only
		auto check = [&](Model::NodeIdType id, ChangeDependencyGraph& cdg) -> bool
		{
			bool atMostOneBranchModifiesBeyondLabel = true;
			if (cdg.changes().contains(id))
			{
				auto elemChange1 = cdg.changes().value(id);
				if (!elemChange1->onlyLabelChange())
				{
					// branch of CDG changes more than label
					for (auto elemChange2 : conflictPairs.values(elemChange1))
					{
						if (elemChange2->nodeId() == elemChange1->nodeId() && !elemChange2->onlyLabelChange())
						{
							atMostOneBranchModifiesBeyondLabel = false;
							break;
						}
					}
				}
			}
			return atMostOneBranchModifiesBeyondLabel;
		};
		bool allElementsMergable = true;
		for (auto id : allElementIds)
		{
			allElementsMergable &= check(id, cdgA) && check(id, cdgB);
			if (!allElementsMergable) break;
		}

		if (!allElementsMergable) continue;

		listsToMerge.insert(changeA->nodeId());

		// TODO extract each check into its own method
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

std::shared_ptr<const ChangeDescription> ListMergeComponent::copyWithNewIndex(
		std::shared_ptr<const ChangeDescription>& change, int index)
{
	GenericNode* newNode = change->nodeB()->persistentUnit()->newNode();
	newNode->reset(change->nodeB());
	newNode->setLabel(QString(index));
	auto newChange = std::make_shared<const ChangeDescription>(change->nodeA(), newNode);

	// change is the same except maybe label flag
	Q_ASSERT(change->type() == newChange->type());
	Q_ASSERT((change->flags() | ChangeDescription::Label) == (newChange->flags() | ChangeDescription::Label));
	return newChange;
}

} /* namespace FilePersistence */
