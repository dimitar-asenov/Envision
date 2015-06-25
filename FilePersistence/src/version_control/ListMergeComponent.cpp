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

	QHash<Model::NodeIdType, QList<Chunk*>> preparedLists;

	for (auto listContainerId : listsToMerge)
	{
		auto listContainerBase = treeBase->find(listContainerId);
		auto idListBase = nodeListToSortedIdList(listContainerBase->children());
		auto idListA = nodeListToSortedIdList(treeA->find(listContainerId)->children());
		auto idListB = nodeListToSortedIdList(treeB->find(listContainerId)->children());

		if (listTypes_.contains(listContainerBase->type()))
		{
			// list is ordered
			auto chunks = Diff3Parse::computeChunks(idListA, idListB, idListBase);
			preparedLists.insert(listContainerId, chunks);
		}
		else // list is unordered
		{
			/*
			QList<Model::NodeIdType> listInChunk;
			listInChunk.append(idListA);
			for (auto elem : idListB)
				if (!listInChunk.contains(elem))
				{
					auto pos = findPosition(elem, idListB, listInChunk);
					if (pos.predecessor_.isNull()) listInChunk.prepend(elem);
					else
					{
						int idx = listInChunk.indexOf(pos.predecessor_);
						listInChunk.insert(idx + 1, elem);
					}
				}
			auto unorderedChunk = new Chunk(true, listInChunk);
			mergedList->append(unorderedChunk);
			*/
		}
	}

	for (auto preparedListIt = preparedLists.begin(); preparedListIt != preparedLists.end(); ++preparedListIt)
	{
		for (auto chunk : preparedListIt.value())
			if (!chunk->stable_)
				computeMergedChunk(chunk, cdgA, cdgB);
	}

	for (auto preparedListIt = preparedLists.begin(); preparedListIt != preparedLists.end(); ++preparedListIt)
	{
		QList<Model::NodeIdType> mergedList;
		for (auto chunk : preparedListIt.value())
		{
			if (chunk->noConflicts_)
				mergedList.append(chunk->spanMerged_);
		}

		// assert that each element occurs only once in the merged list.
		for (auto elemId : mergedList) Q_ASSERT(mergedList.indexOf(elemId) == mergedList.lastIndexOf(elemId));

		// TODO update transition instead of overwrite
		transition = translateListIntoChanges(treeA, treeB, treeBase, preparedListIt.key(),
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

void ListMergeComponent::computeMergedChunk(Chunk* chunk,
														  ChangeDependencyGraph& cdgA,
														  ChangeDependencyGraph& cdgB)
{
	bool valid = true;
	valid &= insertElemsIntoChunk(chunk, chunk->spanBase_,
											cdgA, cdgB, chunk->spanA_, chunk->spanB_, true);
	if (valid)
		valid &= insertElemsIntoChunk(chunk, chunk->spanBase_,
												cdgB, cdgA, chunk->spanB_, chunk->spanA_, false);
	Q_ASSERT(chunk->noConflicts_ == valid);
	// mergedChunk->noConflicts_ = valid;
}

bool ListMergeComponent::insertElemsIntoChunk(Chunk* chunk,
															 const QList<Model::NodeIdType>& spanBase,
															 const ChangeDependencyGraph& cdgA,
															 const ChangeDependencyGraph& cdgB,
															 const QList<Model::NodeIdType>& spanA,
															 const QList<Model::NodeIdType>& spanB,
															 bool branchIsA)
{
	bool conflict = false;
	for (auto elem : spanA)
	{
		Position posA = findPosition(elem, spanA, chunk);
		Position posB = spanB.contains(elem) ? findPosition(elem, spanB, chunk) : Position(false, {});
		Position posBase = spanBase.contains(elem) ? findPosition(elem, spanBase, chunk) : Position(false, {});

		auto changeA = cdgA.changes().value(elem);
		auto changeB = cdgB.changes().value(elem);

		bool elemInTreeBase = !changeA || changeA->type() != ChangeType::Insertion;
		bool elemInListBase = !changeA || changeA->type() == ChangeType::Stationary;
		bool elemInTreeB = elemInTreeBase && (!changeB || changeB->type() != ChangeType::Deletion);
		bool elemInListB = false; // FIXME find a way to find this.

		auto reorderedNodesByMe = branchIsA ? reorderedNodesA_ : reorderedNodesB_;
		auto reorderedNodesByOther = branchIsA ? reorderedNodesB_ : reorderedNodesA_;
		auto mustBeUnchangedByMe = branchIsA ? mustBeUnchangedByA_ : mustBeUnchangedByB_;
		auto mustBeUnchangedByOther = branchIsA ? mustBeUnchangedByB_ : mustBeUnchangedByA_;

		/**
		 * If true and the positions are valid, we should insert.
		 * This is false if other branch deletes or element has already been inserted by other branch.
		 */
		bool shouldInsert = false;

		// This is the decision tree. The bit digits describe the path. TODO link to report.
		if (spanBase.contains(elem))
		{
			if (spanB.contains(elem))
			{
				if (posA == posB)
				{
					if (!chunk->spanMerged_.contains(elem)) shouldInsert = true; // 1110
				}
				else
				{
					// NOTE Is this correct, considering how the != operator works for unvalid positions?
					if (posA != posBase)
					{
						markElementAsReordered(reorderedNodesByMe, mustBeUnchangedByMe, elem);
						// NOTE the following condition could be evaluated immediately.
						// all original chunks of the current list are required.
						if (doesOtherBranchReorder(reorderedNodesByOther, mustBeUnchangedByOther, chunk, elem))
							shouldInsert = true; // 11001
						else
						{
							conflict = true;
							break; // 11000
						}
					}
				}
			}
			else
			{
				if (posA != posBase)
				{
					markElementAsReordered(reorderedNodesByMe, mustBeUnchangedByMe, elem);
					conflict = true;
					break; // 100
				}
			}
		}
		else
		{
			if (elemInTreeB)
			{
				if (elemInListB)
				{
					if (spanB.contains(elem))
					{
						if (posA == posB)
						{
							if (!chunk->spanMerged_.contains(elem)) shouldInsert = true; // 011110
						}
						else
						{
							conflict = true;
							break; // 01110
						}
					}
					else
					{
						markElementAsReordered(reorderedNodesByMe, mustBeUnchangedByMe, elem);
						// NOTE the following condition could be evaluated immediately.
						// all original chunks of the current list are required.
						if (!doesOtherBranchReorder(reorderedNodesByOther, mustBeUnchangedByOther, chunk, elem))
							shouldInsert = true; // 011011
						else
						{
							conflict = true;
							break; // 01100 and 011010
						}
					}
				}
				else
				{
					if (elemInListBase)
					{
						markElementAsReordered(reorderedNodesByMe, mustBeUnchangedByMe, elem);
						conflict = true;
						break; // 0101
					}
					else
					{
						if (doesOtherBranchReorder(reorderedNodesByOther, mustBeUnchangedByOther, chunk, elem))
						{
							conflict = true;
							break; // 01001
						}
						else
						{
							shouldInsert = true; // 01000
						}
					}
				}
			}
			else
			{
				if (elemInTreeBase)
				{
					conflict = true;
					break; // 001
				}
				else shouldInsert = true; // 000
			}
		}
		// End decision tree

		if (shouldInsert)
		{
			if (posA.valid_)
				insertAfter(elem, posA, chunk);
			else
				conflict = true;
		}

		if (conflict) break;
	}
	return !conflict;
}

void ListMergeComponent::markElementAsReordered(QSet<Model::NodeIdType>& reorderedNodesByMe,
																QHash<Model::NodeIdType, Chunk*>& mustBeUnchangedByMe,
																Model::NodeIdType elem)
{
	for (auto chunk : mustBeUnchangedByMe.values(elem))
		chunk->noConflicts_ = false; // TODO do this recursively
	reorderedNodesByMe.insert(elem);
}

bool ListMergeComponent::doesOtherBranchReorder(QSet<Model::NodeIdType>& reorderedNodesByOther,
									 QHash<Model::NodeIdType, Chunk*>& mustBeUnchangedByOther,
									 Chunk* chunk, Model::NodeIdType elem)
{
	mustBeUnchangedByOther.insert(elem, chunk);
	return reorderedNodesByOther.contains(elem);
}

ListMergeComponent::Position ListMergeComponent::findPosition(const Model::NodeIdType& element,
																				  const QList<Model::NodeIdType>& origin,
																				  const Chunk* chunk)
{
	// position in empty list is unique
	if (chunk->spanMerged_.isEmpty()) return Position(true, {});

	int elemIdx = origin.indexOf(element) - 1;
	while (elemIdx >= 0 && !chunk->spanMerged_.contains(origin.at(elemIdx))) --elemIdx;
	auto commonPredecessor = (elemIdx >= 0) ? origin.at(elemIdx) : Model::NodeIdType();
	elemIdx = origin.indexOf(element) + 1;
	while (elemIdx < origin.size() && !chunk->spanMerged_.contains(origin.at(elemIdx))) ++elemIdx;
	auto commonSuccessor = (elemIdx < origin.size()) ? origin.at(elemIdx) : Model::NodeIdType();
	int commonPredIdx = chunk->spanMerged_.indexOf(commonPredecessor);
	int commonSuccIdx = chunk->spanMerged_.indexOf(commonSuccessor);

	if (commonPredIdx + 1 == commonSuccIdx) return Position(true, commonPredecessor);
	else return Position(false, commonPredecessor);
}


void ListMergeComponent::insertAfter(Model::NodeIdType elem, Position pos, Chunk* chunk)
{
	Q_ASSERT(pos.valid_);
	if (pos.predecessor_.isNull()) chunk->spanMerged_.prepend(elem);
	else
	{
		int idx = chunk->spanMerged_.indexOf(pos.predecessor_);
		chunk->spanMerged_.insert(idx + 1, elem);
	}
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
