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
#include "Utils.h"

namespace FilePersistence {

ListMergeComponent::ListMergeComponent(QSet<QString>& conflictTypes, QSet<QString>& listTypes,
													QSet<QString>& unorderedTypes) :
	conflictTypes_{conflictTypes}, listTypes_{listTypes}, unorderedTypes_{unorderedTypes} {}

ListMergeComponent::~ListMergeComponent() {}

RelationAssignmentTransition ListMergeComponent::run(const std::unique_ptr<GenericTree>& treeA,
								const std::unique_ptr<GenericTree>& treeB,
								const std::unique_ptr<GenericTree>& treeBase,
								ChangeDependencyGraph& cdgA,
								ChangeDependencyGraph& cdgB,
								QSet<std::shared_ptr<const ChangeDescription> >& conflictingChanges,
								ConflictPairs& conflictPairs, RelationAssignment& relationAssignment)
{
	RelationAssignmentTransition transition(relationAssignment);

	auto listsToMerge = computeListsToMerge(cdgA, cdgB, conflictingChanges, conflictPairs);

	for (auto listContainer : listsToMerge)
	{
		// TODO load from revision
		auto idListA = nodeListToIdList(cdgA.changes().find(listContainer->id()).value()->nodeB()->children());
		auto idListB = nodeListToIdList(cdgB.changes().find(listContainer->id()).value()->nodeB()->children());
		auto idListBase = nodeListToIdList(listContainer->children());
		QList<Model::NodeIdType> mergedList;
		if (listTypes_.contains(listContainer->type()))
		{
			auto chunks = computeChunks(idListA, idListB, idListBase);
			for (auto chunk : chunks)
			{
				if (chunk.stable_)
				{
					mergedList.append(chunk.spanBase_);
				}
				else
				{
					auto mergedChunk = computeMergedChunk(chunk, cdgA, cdgB, idListA, idListB, idListBase);
					if (mergedChunk.valid_) mergedList.append(mergedChunk.chunk_);
					// else mergedChunks.append(chunk.spanBase_); TODO not sure if this is the best thing to do, maybe keep empty
				}
			}
		}
		else // list is unordered
		{
			mergedList.append(idListA);
			for (auto id : idListB)
			{
				if (!mergedList.contains(id)) mergedList.append(id);
			}
		}

		// assert that each element occurs only once in the merged list.
		for (auto elemId : mergedList) Q_ASSERT(mergedList.indexOf(elemId) == mergedList.lastIndexOf(elemId));

		// TODO update transition instead of overwrite
		transition = translateListIntoChanges(listContainer->id(), mergedList, treeA, treeB,
														  treeBase, cdgA, cdgB, relationAssignment);
	}

	return transition;
}

/**
 * Finds all lists with a conflict that may be resolved by the component.
 */
QSet<const GenericNode*> ListMergeComponent::computeListsToMerge(
		ChangeDependencyGraph& cdgA,
		ChangeDependencyGraph& cdgB,
		QSet<std::shared_ptr<const ChangeDescription> >& conflictingChanges,
		ConflictPairs& conflictPairs)
{
	QSet<const GenericNode*> listsToMerge;
	for (auto change : conflictingChanges)
	{
		if (!change->onlyStructureChange()) continue;
		if (!listTypes_.contains(change->nodeA()->type())) continue;

		// node of list type and one branch only changes child structure
		// look for change of other branch of same node
		std::shared_ptr<const ChangeDescription> conflictingChange;
		bool conflictingChangeFound = false;
		for (auto other : conflictPairs.values(change))
		{
			if (other->id() == change->id())
			{
				conflictingChange = other;
				conflictingChangeFound = true;
				break;
			}
		}

		if (conflictingChangeFound && !conflictingChange->onlyStructureChange()) continue;

		// other branch only changes child structure
		// FIXME make clear which change comes from which branch
		QSet<Model::NodeIdType> allElementIds;
		bool allElementsConflictRoots = true;
		for (auto element : Utils::loadChildren(change->nodeA(), revisionIdBase_))
		{
			allElementIds.insert(element->id());
			allElementsConflictRoots &= conflictTypes_.contains(element->type());
			if (!allElementsConflictRoots) break;
		}
		for (auto element : Utils::loadChildren(change->nodeB(), revisionIdA_))
		{
			allElementIds.insert(element->id());
			allElementsConflictRoots &= conflictTypes_.contains(element->type());
			if (!allElementsConflictRoots) break;
		}
		for (auto element : Utils::loadChildren(conflictingChange->nodeB(), revisionIdB_))
		{
			allElementIds.insert(element->id());
			allElementsConflictRoots &= conflictTypes_.contains(element->type());
			if (!allElementsConflictRoots) break;
		}

		if (!allElementsConflictRoots) continue;

		// for all elements, check that conflicts are on label only
		auto check = [&](Model::NodeIdType id, ChangeDependencyGraph& cdg) -> bool
		{
			bool notBothBranchesModify = true;
			if (cdg.changes().contains(id))
			{
				std::shared_ptr<const ChangeDescription> elemChange1 = cdg.changes().value(id);
				if (!elemChange1->onlyLabelChange())
				{
					// branch A changes more than label
					for (auto elemChange2 : conflictPairs.values(elemChange1))
					{
						if (elemChange2->id() == elemChange1->id() && !elemChange2->onlyLabelChange())
						{
							notBothBranchesModify = false;
							break;
						}
					}
				}
			}
			return notBothBranchesModify;
		};
		bool allElementsMergable = true;
		for (auto id : allElementIds)
		{
			allElementsMergable &= check(id, cdgA) && check(id, cdgB);
			if (!allElementsMergable) break;
		}

		if (!allElementsMergable) continue;

		listsToMerge.insert(change->nodeA());
	}
	return listsToMerge;
}

ListMergeComponent::ChunkMergeResult ListMergeComponent::computeMergedChunk(Chunk chunk,
																									 ChangeDependencyGraph& cdgA,
																									 ChangeDependencyGraph& cdgB,
																									 QList<Model::NodeIdType>&,
																									 QList<Model::NodeIdType>& idListB,
																									 QList<Model::NodeIdType>& idListBase)
{
	QList<Model::NodeIdType> mergedChunk;
	bool noConflict = true;
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
						else
						{
							// conflict
							noConflict = false;
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
					if (chunk.spanB_.contains(elem))
					{
						if (posA == posB)
						{
							if (!mergedChunk.contains(elem)) insertAfter(elem, posA, mergedChunk);
						}
						else
						{
							// conflict
							noConflict = false;
							break;
						}
					}
					else
					{
						// conflict
						noConflict = false;
						break;
					}
				}
				else
				{
					if (idListBase.contains(elem))
					{
						// conflict
						noConflict = false;
						break;
					}
					else insertAfter(elem, posA, mergedChunk);
				}
			}
			else
			{
				if (elemInTreeBase)
				{
					// conflict
					noConflict = false;
					break;
				}
				else insertAfter(elem, posA, mergedChunk);
			}
		}
	}
	return ChunkMergeResult(noConflict, mergedChunk);
}

/**
 * Tries to find a unique position for \a elem in \a into that is similar to the position of \a elem in \a from.
 * Returns a Position \a pos where \a pos.valid = true if and only if such a position could be found and pos.predecessor
 * is the element after which \a elem should be inserted or 0 if elem should be inserted at the beginning.
 * Such a position can be found if the nearest predecessor and successor of \elem in \a from that are common in \a into
 * are next to each other and in order in \a into.
 */
ListMergeComponent::Position ListMergeComponent::findPosition(Model::NodeIdType element,
																				  QList<Model::NodeIdType> from, QList<Model::NodeIdType> into)
{
	// position in empty list is unique
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

QList<ListMergeComponent::Chunk> ListMergeComponent::computeChunks(const QList<Model::NodeIdType> idListA,
																								 const QList<Model::NodeIdType> idListB,
																								 const QList<Model::NodeIdType> idListBase)
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

QList<Model::NodeIdType> ListMergeComponent::nodeListToIdList(const QList<GenericNode*>& list) const
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

/**
 * Takes the merged version of a list and generates all changes needed to bring the base version to the merged version.
 * New changes are created in \a cdgA.
 */
RelationAssignmentTransition ListMergeComponent::translateListIntoChanges(Model::NodeIdType,
																								  QList<Model::NodeIdType>& mergedList,
																								  const std::unique_ptr<GenericTree>&,
																								  const std::unique_ptr<GenericTree>&,
																								  const std::unique_ptr<GenericTree>&,
																								  ChangeDependencyGraph& cdgA,
																								  ChangeDependencyGraph& cdgB,
																								  RelationAssignment&)
{
	for (auto elemId : mergedList)
	{
		auto changeAIt = cdgA.changes().find(elemId);
		auto changeBIt = cdgB.changes().find(elemId);
		auto changeA = changeAIt == cdgA.changes().end() ? nullptr : changeAIt.value();
		auto changeB = changeBIt == cdgB.changes().end() ? nullptr : changeBIt.value();
		if (changeA && !changeA->onlyLabelChange())
		{
			// branch A changes node beyond label
			if (changeB) Q_ASSERT(changeB->onlyLabelChange());
			GenericNode* newNode = changeA->nodeB()->persistentUnit()->newNode();
			newNode->setFieldsLike(changeA->nodeB());
			newNode->setName(QString(mergedList.indexOf(elemId)));
			auto newChange = std::make_shared<const ChangeDescription>(changeA->nodeA(), newNode);
			Q_ASSERT(changeA->type() == newChange->type());
			cdgA.replace(changeA, newChange);
		}
		else if (changeB && !changeB->onlyLabelChange())
		{
			// branch A changes node beyond label
			if (changeA) Q_ASSERT(changeA->onlyLabelChange());
			GenericNode* newNode = changeB->nodeB()->persistentUnit()->newNode();
			newNode->setFieldsLike(changeB->nodeB());
			newNode->setName(QString(mergedList.indexOf(elemId)));
			auto newChange = std::make_shared<const ChangeDescription>(changeB->nodeA(), newNode);
			Q_ASSERT(changeB->type() == newChange->type());
			cdgB.replace(changeB, newChange);
		}
		else
		{
			// TODO load node from base or A
			const GenericNode* oldNode = nullptr;
			GenericNode* newNode = oldNode->persistentUnit()->newNode();
			newNode->setFieldsLike(oldNode);
			newNode->setName(QString(mergedList.indexOf(elemId)));
			auto newChange = std::make_shared<const ChangeDescription>(oldNode, newNode);
			if (changeA) cdgA.replace(changeA, newChange);
			else cdgA.insert(newChange);
			// TODO add dependencies.
		}

		// if no branch changes node beyond label, we must construct a new change
		// remove conflicts (here or later?)
	}
	// TODO fill properly
	return RelationAssignmentTransition();
}

} /* namespace FilePersistence */
