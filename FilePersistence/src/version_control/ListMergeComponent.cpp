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

LinkedChangesTransition __attribute__((optimize("O0"))) ListMergeComponent::run(std::shared_ptr<GenericTree>& treeA,
																std::shared_ptr<GenericTree>& treeB,
																std::shared_ptr<GenericTree>& treeBase,
																ChangeDependencyGraph& cdgA,
																ChangeDependencyGraph& cdgB,
																QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
																ConflictPairs& conflictPairs, LinkedChangesSet& linkedChangesSet)
{
	LinkedChangesTransition transition(linkedChangesSet, cdgA, cdgB);

	auto listsToMerge = computeListsToMerge(cdgA, cdgB, conflictingChanges, conflictPairs);

	preparedLists_ = {};

	for (auto listContainerId : listsToMerge)
	{
		auto listContainerBase = treeBase->find(listContainerId, true);
		auto idListBase = nodeListToSortedIdList(listContainerBase->children());
		auto idListA = nodeListToSortedIdList(treeA->find(listContainerId, true)->children());
		auto idListB = nodeListToSortedIdList(treeB->find(listContainerId, true)->children());

		auto chunks = Diff3Parse::computeChunks(idListA, idListB, idListBase);
		preparedLists_.insert(listContainerId, chunks);
	}

	for (auto preparedListIt = preparedLists_.begin(); preparedListIt != preparedLists_.end(); ++preparedListIt)
	{
		for (auto chunk : preparedListIt.value())
			if (!chunk->stable_)
				computeMergedChunk(chunk, preparedListIt.key(), cdgA, cdgB);
	}

	for (auto preparedListIt = preparedLists_.begin(); preparedListIt != preparedLists_.end(); ++preparedListIt)
	{
		QList<Model::NodeIdType> mergedList;
		bool allResolved = true;
		for (auto chunk : preparedListIt.value())
		{
			if (chunk->noConflicts_)
			{
				mergedList.append(chunk->spanMerged_);
				// find deletions
				for (auto elem : chunk->spanBase_)
				{
					if (!chunk->spanMerged_.contains(elem))
					{
						auto changeA = cdgA.changes().value(elem);
						auto changeB = cdgB.changes().value(elem);
						if (changeA && changeA->type() == ChangeType::Deletion)
							markAsResolved(conflictingChanges, conflictPairs, changeA, cdgA, cdgB);
						else if (changeB && changeB->type() == ChangeType::Deletion)
							markAsResolved(conflictingChanges, conflictPairs, changeB, cdgB, cdgA);
					}
				}
			}
			else
				allResolved = false;
		}

		// assert that each element occurs only once in the merged list.
		for (auto elemId : mergedList) Q_ASSERT(mergedList.indexOf(elemId) == mergedList.lastIndexOf(elemId));

		// TODO update transition instead of overwrite
		transition = translateListIntoChanges(treeA, treeB, treeBase, preparedListIt.key(),
														  mergedList, cdgA, cdgB, linkedChangesSet,
														  conflictingChanges, conflictPairs);
		if (allResolved)
		{
			auto containerChange = cdgA.changes().value(preparedListIt.key());
			markAsResolved(conflictingChanges, conflictPairs, containerChange, cdgA, cdgB);
		}
	}

	return transition;
}

Chunk* ListMergeComponent::findOriginalChunk(Model::NodeIdType element, Model::NodeIdType listContainer, Chunk* guess)
{
	if (guess && guess->spanBase_.contains(element))
			return guess;
	else
	{
		for (auto chunk : preparedLists_.value(listContainer))
		{
			if (chunk->spanBase_.contains(element))
				return chunk;
		}
	}
	return nullptr;
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
	if (changeA)
		allElementsConflictRoots &= gatherAndCheckElems(changeA->nodeA());
	else
		allElementsConflictRoots &= gatherAndCheckElems(changeB->nodeA());

	if (changeA)
		allElementsConflictRoots &= gatherAndCheckElems(changeA->nodeB());
	if (changeB)
		allElementsConflictRoots &= gatherAndCheckElems(changeB->nodeB());

	return {allElementsConflictRoots, allElementIds};
}

bool __attribute__((optimize("O0"))) onlyConflictsOnLabel(ConflictPairs& conflictPairs,
																			 QSet<Model::NodeIdType>& allElementIds,
																			 ChangeDependencyGraph& cdgA)
{
	for (auto elem : allElementIds)
	{
		auto changeA = cdgA.changes().value(elem);
		if (changeA)
		{
			for (auto changeB : conflictPairs.values(changeA))
			{
				if (!((changeA->onlyLabelChange() && conflictPairs.values(changeB).size() == 1) ||
						(changeB->onlyLabelChange() && conflictPairs.values(changeA).size() == 1)))
					return false;
			}
		}
	}
	return true;
}

QSet<Model::NodeIdType> __attribute__((optimize("O0"))) ListMergeComponent::computeListsToMerge(
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

		if (conflictingChange)
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

		listsToMerge.insert(change->nodeId());
	}
	return listsToMerge;
}

void ListMergeComponent::computeMergedChunk(Chunk* chunk,
														  const Model::NodeIdType containerId,
														  ChangeDependencyGraph& cdgA,
														  ChangeDependencyGraph& cdgB)
{
	bool valid = true;
	valid &= insertElemsIntoChunk(chunk, chunk->spanBase_, containerId,
											cdgA, cdgB, chunk->spanA_, chunk->spanB_, true);
	if (valid)
		valid &= insertElemsIntoChunk(chunk, chunk->spanBase_, containerId,
												cdgB, cdgA, chunk->spanB_, chunk->spanA_, false);
	Q_ASSERT(chunk->noConflicts_ == valid);
	// mergedChunk->noConflicts_ = valid;
}

bool ListMergeComponent::insertElemsIntoChunk(Chunk* chunk,
															 const QList<Model::NodeIdType>& spanBase,
															 const Model::NodeIdType containerId,
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
		bool elemInListB = (changeA && changeA->type() == ChangeType::Move) ?
					changeB && changeB->nodeB()->parentId() == changeA->nodeB()->parentId() :
					!changeB || changeB->type() != ChangeType::Move;
		// If A moves, B must move to same list, otherwise B must not move.

		bool listIsOrdered = listTypes_.contains(cdgA.changes().value(containerId)->nodeA()->type());

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
				if (listIsOrdered)
				{
					if (posA == posB)
					{
						if (!chunk->spanMerged_.contains(elem))
							shouldInsert = true; // 1110
					}
					else
					{
						// NOTE Is this correct, considering how the != operator works for invalid positions?
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
					// list is unordered and both braches have elem in same chunk.
					// just insert at good-enough position.
					// This is for the whole 11 subtree.
					if (!chunk->spanMerged_.contains(elem))
						shouldInsert = true;
				}
			}
			else
			{
				if (posA != posBase)
				{
					if (listIsOrdered)
					{
						markElementAsReordered(reorderedNodesByMe, mustBeUnchangedByMe, elem);
						conflict = true;
						break; // 100
					}
					// else B reorders across chunk in unordered list. Let B handle it.
				}
			}
		}
		else
		{
			if (elemInTreeB)
			{
				if (elemInListB)
				{
					if (listIsOrdered)
					{
						if (spanB.contains(elem))
						{
							if (posA == posB)
							{
								if (!chunk->spanMerged_.contains(elem))
								{
									// 011110
									auto origin = findOriginalChunk(elem, containerId);
									if (!origin)
										shouldInsert = true;
									else if (origin->noConflicts_)
									{
										chunkDependencies_.insert(chunk, origin);
										chunkDependencies_.insert(origin, chunk);
										shouldInsert = true;
									}
									else
									{
										conflict = true;
										break; // the chunk this chunk depends on is conflicting
									}
								}
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
							{
								// 011011
								auto origin = findOriginalChunk(elem, containerId);
								if (!origin)
									shouldInsert = true;
								else if (origin->noConflicts_)
								{
									chunkDependencies_.insert(chunk, origin);
									chunkDependencies_.insert(origin, chunk);
									shouldInsert = true;
								}
								else
								{
									conflict = true;
									break; // the chunk this chunk depends on is conflicting
								}
							}
							else
							{
								conflict = true;
								break; // 01100 and 011010
							}
						}
					}
					else
					{
						// list is unordered and both braches have elem in the list.
						// this branch has elem in a different chunk than base.
						// just insert at good-enough position.
						// This is for the whole 011 subtree.
						if (!chunk->spanMerged_.contains(elem))
							shouldInsert = true;
					}
				}
				else
				{
					if (elemInListBase)
					{
						if (listIsOrdered)
						{
							markElementAsReordered(reorderedNodesByMe, mustBeUnchangedByMe, elem);
							conflict = true;
							break; // 0101
						}
						// else A reorders in unordered list, B moves out. Move takes precedence.
					}
					else
					{
						if (doesOtherBranchReorder(reorderedNodesByOther, mustBeUnchangedByOther, chunk, elem))
						{
							if (listIsOrdered)
							{
								conflict = true;
								break; // 01001
							}
							else
							{
								auto origin = findOriginalChunk(elem, containerId);
								if (!origin)
									shouldInsert = true;
								else if (origin->noConflicts_)
								{
									chunkDependencies_.insert(chunk, origin);
									chunkDependencies_.insert(origin, chunk);
									shouldInsert = true;
									// A moves into unordered lists, B reorders. Move takes precedence.
								}
								else
								{
									conflict = true;
									break; // the chunk this chunk depends on is conflicting
								}
							}
						}
						else
						{
							// 01000
							auto origin = findOriginalChunk(elem, containerId);
							if (!origin)
								shouldInsert = true;
							else if (origin->noConflicts_)
							{
								chunkDependencies_.insert(chunk, origin);
								chunkDependencies_.insert(origin, chunk);
								shouldInsert = true;
							}
							else
							{
								conflict = true;
								break; // the chunk this chunk depends on is conflicting
							}
						}
					}
				}
			}
			else
			{
				if (elemInTreeBase)
				{
					if (listIsOrdered)
					{
						conflict = true;
						break; // 001
					}
					else
					{
						if (changeA->type() == ChangeType::Move)
						{
							conflict = true;
							break; // A moves, B deletes
						}
						// else A reorders in unordered lists, B deletes. Delete takes precedence.
					}
				}
				else shouldInsert = true; // 000
			}
		}
		// End decision tree

		if (conflict) break;

		if (shouldInsert)
		{
			if (!listIsOrdered || posA.valid_)
				insertAfter(elem, posA, chunk);
			else
			{
				conflict = true;
				break;
			}
		}
	}

	if (conflict)
		markDependingAsConflicting(chunk);

	return !conflict;
}

void ListMergeComponent::markDependingAsConflicting(Chunk* chunk)
{
	QList<Chunk*> queue = {chunk};
	while (!queue.isEmpty())
	{
		auto chunk = queue.takeFirst();
		chunk->noConflicts_ = false;
		for (auto depending : chunkDependencies_.values(chunk))
		{
			if (depending->noConflicts_)
				queue.append(depending);
		}
	}
}

void ListMergeComponent::markElementAsReordered(QSet<Model::NodeIdType>& reorderedNodesByMe,
																QHash<Model::NodeIdType, Chunk*>& mustBeUnchangedByMe,
																Model::NodeIdType elem)
{
	for (auto chunk : mustBeUnchangedByMe.values(elem))
	{
		chunk->noConflicts_ = false;
		markDependingAsConflicting(chunk);
	}
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

	int elemIdx = origin.indexOf(element) - 1; // set index to predecessor
	while (elemIdx >= 0 && !chunk->spanMerged_.contains(origin.at(elemIdx)))
		--elemIdx; // go backward until common element is found
	auto commonPredecessor = (elemIdx >= 0) ? origin.at(elemIdx) : Model::NodeIdType();
	elemIdx = origin.indexOf(element) + 1; // set index to successor
	while (elemIdx < origin.size() && !chunk->spanMerged_.contains(origin.at(elemIdx)))
		++elemIdx; // go forward until common element is found
	auto commonSuccessor = (elemIdx < origin.size()) ? origin.at(elemIdx) : Model::NodeIdType();
	int commonPredIdx = chunk->spanMerged_.indexOf(commonPredecessor);
	int commonSuccIdx = chunk->spanMerged_.indexOf(commonSuccessor);

	if ((commonPredIdx + 1 == commonSuccIdx) || //below condition is to append at end
		 (commonPredIdx == chunk->spanMerged_.size() - 1 && commonSuccIdx == -1))
		return Position(true, commonPredecessor);
	else
		return Position(false, commonPredecessor);
}


void ListMergeComponent::insertAfter(Model::NodeIdType elem, Position pos, Chunk* chunk)
{
	if (pos.predecessor_.isNull()) chunk->spanMerged_.prepend(elem);
	else
	{
		int idx = chunk->spanMerged_.indexOf(pos.predecessor_);
		Q_ASSERT(idx >= 0);
		chunk->spanMerged_.insert(idx + 1, elem);
	}
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


bool ListMergeComponent::noConflictingDependencies(ChangeDependencyGraph& cdg,
																	QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
																	std::shared_ptr<ChangeDescription>& change)
{
	for (auto dependency : cdg.getDependencies(change))
	{
		if (conflictingChanges.contains(dependency))
			return false;
	}
	return true;
}

void ListMergeComponent::markDependingAsResolved(ChangeDependencyGraph& cdg,
																 QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
																 ConflictPairs& conflictPairs, std::shared_ptr<ChangeDescription>& change)
{
	for (auto depending : cdg.getDependendingChanges(change))
	{
		if (conflictPairs.values(depending).isEmpty())
		{
			conflictingChanges.remove(depending);
			if (noConflictingDependencies(cdg, conflictingChanges, depending))
				markDependingAsResolved(cdg, conflictingChanges, conflictPairs, depending);
		}
	}
}

void ListMergeComponent::markAsResolved(QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
														  ConflictPairs& conflictPairs, std::shared_ptr<ChangeDescription> change,
														  ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB)
{
	std::shared_ptr<ChangeDescription> conflictingSameId;
	// remove all conflict pairs and find pair change.
	for (auto other : conflictPairs.values(change))
	{
		conflictPairs.remove(change, other);
		if (other->nodeId() == change->nodeId())
			conflictingSameId = other;
	}
	// check if applicable and if yes, mark dependending changes as resolved.
	if (noConflictingDependencies(cdgA, conflictingChanges, change))
	{
		conflictingChanges.remove(change);
		markDependingAsResolved(cdgA, conflictingChanges, conflictPairs, change);
	}

	// if pair change exists, remove it
	if (conflictingSameId)
	{
		Q_ASSERT(cdgB.changes().value(change->nodeId()) == conflictingSameId);
		for (auto other : conflictPairs.values(conflictingSameId))
			conflictPairs.remove(conflictingSameId, other);
		conflictingChanges.remove(conflictingSameId);
		cdgB.remove(conflictingSameId);
	}
}

LinkedChangesTransition ListMergeComponent::translateListIntoChanges(
		std::shared_ptr<GenericTree> treeA, std::shared_ptr<GenericTree>,
		std::shared_ptr<GenericTree> treeBase, Model::NodeIdType containerId,
		QList<Model::NodeIdType>& mergedList, ChangeDependencyGraph& cdgA,
		ChangeDependencyGraph& cdgB, LinkedChangesSet& linkedChangesSet,
		QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
		ConflictPairs& conflictPairs)
{
	for (auto elemId : mergedList)
	{
		auto changeA = cdgA.changes().value(elemId);
		auto changeB = cdgB.changes().value(elemId);
		auto updateChange = [&](
				std::shared_ptr<ChangeDescription>& change,
				ChangeDependencyGraph& cdgA,
				ChangeDependencyGraph& cdgB)
		{
			auto node = change->nodeB();
			node->setLabel(QString::number(mergedList.indexOf(elemId)));
			if (node->parentId() != containerId)
			{
				node->detachFromParent();
				node->setParentId(containerId);
				node->attachToParent();
			}
			change->computeFlags();
			markAsResolved(conflictingChanges, conflictPairs, change, cdgA, cdgB);
		};

		if (changeA && !changeA->onlyLabelChange())
		{
			// branch A changes node beyond label
			if (changeB) Q_ASSERT(changeB->onlyLabelChange());
			updateChange(changeA, cdgA, cdgB);
		}
		else if (changeB && !changeB->onlyLabelChange())
		{
			// branch B changes node beyond label
			if (changeA) Q_ASSERT(changeA->onlyLabelChange());
			updateChange(changeB, cdgB, cdgA);
		}
		else if (changeA)
			updateChange(changeA, cdgA, cdgB);
		else if (changeB)
			updateChange(changeB, cdgB, cdgA);
		else
		{
			// no branch changes node beyond label so we must construct a new change
			auto oldNode = treeBase->find(elemId);
			auto newNode = treeA->find(elemId);
			Q_ASSERT(oldNode);
			Q_ASSERT(newNode);
			auto newChange = std::make_shared<ChangeDescription>(oldNode, newNode);
			updateChange(newChange, cdgA, cdgB);
			cdgA.insert(newChange);
			cdgA.recordDependencies(newChange, true);
			cdgA.recordDependencies(newChange, false);
		}
	}
	return LinkedChangesTransition(linkedChangesSet); // TODO fill properly
}

} /* namespace FilePersistence */
