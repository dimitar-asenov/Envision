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

#include "ChangeDependencyGraph.h"
#include "ChangeDescription.h"
#include "ConflictPairs.h"
#include "Diff3Parse.h"
#include "../simple/GenericNode.h"
#include "../simple/GenericTree.h"

namespace FilePersistence {

ListMergeComponent::ListMergeComponent(QSet<QString>& conflictTypes, QSet<QString>& listTypes,
													QSet<QString>& unorderedTypes) :
	conflictTypes_{conflictTypes}, listTypes_{listTypes}, unorderedTypes_{unorderedTypes} {}

ListMergeComponent::~ListMergeComponent() {}

LinkedChangesTransition ListMergeComponent::run(std::shared_ptr<GenericTree>& treeA,
																std::shared_ptr<GenericTree>& treeB,
																std::shared_ptr<GenericTree>& treeBase,
																ChangeDependencyGraph& cdgA,
																ChangeDependencyGraph& cdgB,
																QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
																ConflictPairs& conflictPairs, LinkedChangesSet& linkedChangesSet)
{
	treeA_ = treeA;
	treeB_ = treeB;
	treeBase_ = treeBase;

	LinkedChangesTransition transition{linkedChangesSet, cdgA, cdgB};

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
		for (auto chunk : preparedListIt.value())
			if (!chunk->stable_)
				computeMergedChunk(chunk, preparedListIt.key(), cdgA, cdgB);

	for (auto preparedListIt = preparedLists_.begin(); preparedListIt != preparedLists_.end(); ++preparedListIt)
	{
		// mergedList is only used to check stuff
		QList<Model::NodeIdType> mergedList;
		// this is the index of the next element to be inserted
		int index = 0;
		auto updateChange = [&](
				std::shared_ptr<ChangeDescription>& change,
				ChangeDependencyGraph& cdgA,
				ChangeDependencyGraph& cdgB)
		{
			auto node = change->nodeB();
			node->setLabel(QString::number(index++));
			if (node->parentId() != preparedListIt.key())
			{
				node->detachFromParent();
				node->setParentId(preparedListIt.key());
				node->attachToParent();
			}
			change->computeFlags();
			markAsResolved(conflictingChanges, conflictPairs, change, cdgA, cdgB);
		};

		bool allResolved = true;
		for (auto chunk : preparedListIt.value())
			if (chunk->noConflicts_)
			{
				mergedList.append(chunk->spanMerged_);

				// modify the CDGs and mark the relevant conflicts as resolved

				// find deletions and move-outs first
				for (auto elemId : chunk->spanBase_)
					if (!chunk->spanMerged_.contains(elemId))
					{
						auto changeA = cdgA.changes().value(elemId);
						auto changeB = cdgB.changes().value(elemId);
						if (changeA && (changeA->type() == ChangeType::Deletion ||
											 (changeA->type() == ChangeType::Move &&
											  !preparedLists_.contains(changeA->nodeB()->parentId()))))
							markAsResolved(conflictingChanges, conflictPairs, changeA, cdgA, cdgB);
						else if (changeB && (changeB->type() == ChangeType::Deletion ||
													(changeB->type() == ChangeType::Move &&
													 !preparedLists_.contains(changeB->nodeB()->parentId()))))
							markAsResolved(conflictingChanges, conflictPairs, changeB, cdgB, cdgA);
					}

				for (auto elemId : chunk->spanMerged_)
				{
					auto changeA = cdgA.changes().value(elemId);
					auto changeB = cdgB.changes().value(elemId);

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
			}
			else
			{
				allResolved = false;
				mergedList.append(chunk->spanBase_);
//				index += chunk->spanBase_.size();

				// TODO: Fix the code below, by rethinking what the output of the list merge should be.
				// In particular, we should not remove conflicting changes from the CDG, but are
				// forced to so at the moment since only one change can affect a node, and we need
				// a change to adjust the position of a conflicting element (non-moved) if some
				// deletions/insertions were correctly merged infront of it.
				for (auto elemId : chunk->spanBase_)
				{
					auto oldNode = treeBase->find(elemId);
					auto newNodeA = treeA->find(elemId);
					auto newNodeB = treeB->find(elemId);
					auto adjustChange = [&](GenericNode* oldNode, GenericNode* newNode,
							ChangeDependencyGraph& cdgA,
							ChangeDependencyGraph& cdgB)
					{
						auto newChange = std::make_shared<ChangeDescription>(oldNode, newNode);
						updateChange(newChange, cdgA, cdgB);
						auto delChange = cdgA.changes().value(elemId);
						if (delChange)	cdgA.remove(delChange);		// Remove previous change
						cdgA.insert(newChange);
						cdgA.recordDependencies(newChange, true);
						cdgA.recordDependencies(newChange, false);
					};

					Q_ASSERT(oldNode);
					Q_ASSERT(newNodeA || newNodeB);
					if (newNodeA)	//Add change in cdgA
					{
						adjustChange(oldNode, newNodeA, cdgA, cdgB);
					}
					else{		//Add change in cdgB
						adjustChange(oldNode, newNodeB, cdgB, cdgA);
					}
				}
			}

		// assert that each element occurs only once in the merged list.
		for (int elemIdx = 0; elemIdx < mergedList.size(); ++elemIdx)
		{
			auto elemId = mergedList[elemIdx];
			auto changeA = cdgA.changes().value(elemId);
			auto changeB = cdgB.changes().value(elemId);
			if (changeA && !conflictingChanges.contains(changeA))
				Q_ASSERT(changeA->nodeB()->label().toInt() == elemIdx);
			else if (changeB && !conflictingChanges.contains(changeB))
				Q_ASSERT(changeB->nodeB()->label().toInt() == elemIdx);
			else
				Q_ASSERT(treeBase->find(elemId)->label().toInt() == elemIdx);
		}

		if (allResolved)
		{
			auto containerChange = cdgA.changes().value(preparedListIt.key());
			markAsResolved(conflictingChanges, conflictPairs, containerChange, cdgA, cdgB);
		}
	}

	// TODO implement transitions for this component
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

bool onlyConflictsOnLabel(ConflictPairs& conflictPairs, QSet<Model::NodeIdType>& allElementIds,
								  ChangeDependencyGraph& cdgA, Model::NodeIdType containerId)
{
	for (auto elem : allElementIds)
	{
		auto changeA = cdgA.changes().value(elem);
		std::shared_ptr<ChangeDescription> changeB = nullptr;
		for (auto change : conflictPairs.values(changeA))
			if (change->nodeId() == elem)
			{
				changeB = change;
				break;
			}

		if (changeA && changeB)
		{
			if (changeA->onlyLabelChange() && changeB->onlyLabelChange())	continue;
			else return false;
			(void) containerId;	//	Used for the code described below
//			This part of code contains the original code implemented in Balz thesis
//			Now this condition is relaxed
/*
			if (!changeB->onlyLabelChange())
			{
				if (!changeA->onlyLabelChange())
					return false;
				else
				{
					// changeA is only label change, check conflict pairs for changeB
					for (auto pairedChangeA : conflictPairs.values(changeB))
					{
						if (!((pairedChangeA->nodeA() && pairedChangeA->nodeA()->parentId() == containerId) ||
								(pairedChangeA->nodeB() && pairedChangeA->nodeB()->parentId() == containerId)))
							return false;
					}
				}
			}
			else
			{
				if (!changeB->onlyLabelChange())
					return false;
				else
				{
					// changeB is only label change, check conflict pairs for changeA
					for (auto pairedChangeB : conflictPairs.values(changeA))
					{
						if (!((pairedChangeB->nodeA() && pairedChangeB->nodeA()->parentId() == containerId) ||
								(pairedChangeB->nodeB() && pairedChangeB->nodeB()->parentId() == containerId)))
							return false;
					}
				}
			}
*/
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

		if (!onlyConflictsOnLabel(conflictPairs, allElementIds, cdgA, change->nodeId())) continue;

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
}


bool ListMergeComponent::elementIsStable(const Model::NodeIdType& elem,
													  const QList<Model::NodeIdType>& listA,
													  const QList<Model::NodeIdType>& listB)
{
	int idxB = listB.indexOf(elem);
	int i = 0;
	while (listA[i] != elem)
		if (listB.indexOf(listA[i++], idxB + 1) > 0)
			return false;
	if (idxB > 0)
	{
		++i;
		while (i < listA.size())
			if (listB.lastIndexOf(listA[i++], idxB - 1) > 0)
				return false;
	}
	return true;
}

bool ListMergeComponent::insertElemsIntoChunk(Chunk* chunk,
															 const QList<Model::NodeIdType>& spanBase,
															 const Model::NodeIdType containerId,
															 const ChangeDependencyGraph& cdgThis,
															 const ChangeDependencyGraph& cdgOther,
															 const QList<Model::NodeIdType>& spanThis,
															 const QList<Model::NodeIdType>& spanOther,
															 bool branchIsA)
{
	if (!chunk->noConflicts_)
		return false;

	bool conflict = false;

	for (auto elem : spanThis)
	{
		// first collect boolean variables (complicated for speed)
		Position posA = findPosition(elem, spanThis, chunk);
		Position posB = spanOther.contains(elem) ? findPosition(elem, spanOther, chunk) : Position{false, {}};

		auto changeThis = cdgThis.changes().value(elem);
		auto changeOther = cdgOther.changes().value(elem);

		bool thisReorders = ((changeThis && changeThis->type() != ChangeType::Stationary) ||
									!spanBase.contains(elem) ||
									!elementIsStable(elem, spanThis, spanBase));

		bool otherReorders = (changeOther && changeOther->type() != ChangeType::Stationary);
		if (!otherReorders && !(changeThis && changeThis->type() == ChangeType::Insertion))
		{
			Chunk* otherChunk = nullptr;
			auto parentId = (changeOther ? changeOther->nodeB() : treeBase_->find(elem))->parentId();
			auto otherSpan = branchIsA ? &Chunk::spanB_ : &Chunk::spanA_;
			for (auto chunk : preparedLists_.value(parentId))
				if ((chunk->*otherSpan).contains(elem))
				{
					otherChunk = chunk;
					break;
				}

			otherReorders = (otherChunk && (!otherChunk->spanBase_.contains(elem) ||
								  !elementIsStable(elem, (otherChunk->*otherSpan), otherChunk->spanBase_)));
		}

		bool branchesAgreeOnParent;
		if (changeThis && changeThis->type() == ChangeType::Move)
			branchesAgreeOnParent = changeOther &&
					changeOther->nodeB()->parentId() == changeThis->nodeB()->parentId();
		else
			branchesAgreeOnParent = !changeOther || changeOther->type() != ChangeType::Move;

		bool branchesAgreeOnPosition = branchesAgreeOnParent && posA == posB;
		bool listIsOrdered = listTypes_.contains(cdgThis.changes().value(containerId)->nodeA()->type());

		// begin computing decsision

		bool shouldInsert = false;

		if (thisReorders)
		{
			// the node is not new so there might be dependencies
			// TODO do this better
			auto originChunk = findOriginalChunk(elem, containerId, chunk);
			if (originChunk && originChunk != chunk)
			{
				if (!chunkDependencies_.contains(chunk, originChunk))
				{
					chunkDependencies_.insert(chunk, originChunk);
					chunkDependencies_.insert(originChunk, chunk);
				}

				if (!originChunk->noConflicts_)
				{
					conflict = true;
					break;
				}
			}
		}


		if (changeThis && changeThis->type() == ChangeType::Insertion)
			shouldInsert = true;

		else if (chunk->spanMerged_.contains(elem))
			continue; // do nothing

		else if (thisReorders && otherReorders)
		{
			if (branchesAgreeOnPosition ||
				 (!listIsOrdered && branchesAgreeOnParent))
				shouldInsert = true;
			else
				conflict = true;
		}
		else if (otherReorders)
		{
			// do nothing
			continue;
		}
		else
		{
			// this branch or neither branch reorders
			shouldInsert = true;
		}

		if (conflict) break;

		// decision is made, insert element (and check chunk dependencies)

		Q_ASSERT(shouldInsert);

		if (listIsOrdered && !posA.valid_)
		{
			conflict = true;
			break;
		}

		insertAfter(elem, posA, chunk);
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

ListMergeComponent::Position ListMergeComponent::findPosition(const Model::NodeIdType& element,
																				  const QList<Model::NodeIdType>& origin,
																				  const Chunk* chunk)
{
	// position in empty list is unique
	if (chunk->spanMerged_.isEmpty()) return Position{true, {}};

	int elemIdx = origin.indexOf(element) - 1; // set index to predecessor
	while (elemIdx >= 0 && !chunk->spanMerged_.contains(origin.at(elemIdx)))
		--elemIdx; // go backward until common element is found
	auto commonPredecessor = (elemIdx >= 0) ? origin.at(elemIdx) : Model::NodeIdType{};
	elemIdx = origin.indexOf(element) + 1; // set index to successor
	while (elemIdx < origin.size() && !chunk->spanMerged_.contains(origin.at(elemIdx)))
		++elemIdx; // go forward until common element is found
	auto commonSuccessor = (elemIdx < origin.size()) ? origin.at(elemIdx) : Model::NodeIdType{};
	int commonPredIdx = chunk->spanMerged_.indexOf(commonPredecessor);
	int commonSuccIdx = chunk->spanMerged_.indexOf(commonSuccessor);

	if ((commonPredIdx + 1 == commonSuccIdx) || //below condition is to append at end
		 (commonPredIdx == chunk->spanMerged_.size() - 1 && commonSuccIdx == -1))
		return Position{true, commonPredecessor};
	else
		return Position{false, commonPredecessor};
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

void ListMergeComponent::tryResolve(QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
					 ConflictPairs& conflictPairs, std::shared_ptr<ChangeDescription> change,
					 ChangeDependencyGraph& cdgA)
{
	if (conflictPairs.values(change).isEmpty() &&
		 noConflictingDependencies(cdgA, conflictingChanges, change))
	{
		conflictingChanges.remove(change);
		for (auto dependingChange : cdgA.getDependendingChanges(change))
			tryResolve(conflictingChanges, conflictPairs, dependingChange, cdgA);
	}
}

void ListMergeComponent::markAsResolved(QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
														  ConflictPairs& conflictPairs, std::shared_ptr<ChangeDescription> change,
														  ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB)
{
	// get conflicting change
	auto conflictingSameId = cdgB.changes().value(change->nodeId());
	if (conflictingSameId)
	{
		for (auto pair : conflictPairs.values(conflictingSameId))
			conflictPairs.remove(conflictingSameId, pair);
		conflictingChanges.remove(conflictingSameId);
		cdgB.remove(conflictingSameId);
	}
	for (auto pair : conflictPairs.values(change))
		conflictPairs.remove(change, pair);
	conflictingChanges.remove(change);
	for (auto dep : cdgA.getDependencies(change))
		if (conflictingChanges.contains(dep))
			markAsResolved(conflictingChanges, conflictPairs, dep, cdgA, cdgB);
	markDependingAsResolved(cdgA, conflictingChanges, conflictPairs, change);
}

}
