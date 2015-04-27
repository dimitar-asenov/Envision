/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "Merge.h"

#include "GitRepository.h"

#include "ModelBase/src/model/TreeManager.h"

namespace FilePersistence {

bool Merge::abort()
{
	if (stage_ != Stage::NoMerge && stage_ != Stage::Complete)
	{
		QString branch = repository_->currentBranch();

		repository_->checkout(head_, true);
		repository_->setReferenceTarget(branch, head_);

		stage_ = Stage::NoMerge;
		return true;
	}
	else
		return false;
}

bool Merge::commit(const Signature& author, const Signature& committer, const QString& message)
{
	if (stage_ == Stage::ReadyToCommit)
	{
		QString treeSHA1 = repository_->writeIndexToTree();

		QStringList parents;
		parents.append(head_);
		parents.append(revision_);

		repository_->newCommit(treeSHA1, message, author, committer, parents);

		stage_ = Stage::Complete;

		return true;
	}
	else
		return false;
}

const std::unique_ptr<GenericTree>& Merge::mergeTree() const
{
	return mergeTree_;
}

// ======== private ========

Merge::Merge(QString revision, bool fastForward, GitRepository* repository)
	: fastForward_{fastForward}, repository_{repository}
{
	head_ = repository_->getSHA1("HEAD");
	revision_ = repository_->getSHA1(revision);

	mergeBase_ = repository_->findMergeBase(revision);
	if (mergeBase_.isNull())
		error_ = Error::NoMergeBase;

	stage_ = Stage::Initialized;

	classifyKind();
	performMerge();
}

Merge::~Merge() {}

void Merge::classifyKind()
{
	if (mergeBase_.compare(revision_) == 0)
		kind_ = Kind::AlreadyUpToDate;
	else if (mergeBase_.compare(head_) == 0)
		kind_ = Kind::FastForward;
	else
		kind_ = Kind::TrueMerge;

	stage_ = Stage::Classified;
}

void Merge::performMerge()
{
	switch (kind_)
	{
		case Kind::AlreadyUpToDate:
			stage_ = Merge::Stage::Complete;
			break;

		case Kind::FastForward:
			if (fastForward_)
				performFastForward();
			else
			{
				repository_->checkout(revision_, true);
				repository_->writeRevisionIntoIndex(revision_);
				stage_ = Stage::ReadyToCommit;
			}
			break;

		case Kind::TrueMerge:
		{

			baseHeadDiff_ = repository_->diff(mergeBase_, head_);
			baseRevisionDiff_ = repository_->diff(mergeBase_, revision_);

			mergeBaseTree_ = std::unique_ptr<GenericTree>(new GenericTree("MergeBase", mergeBase_));
			repository_->loadGenericTree(mergeBaseTree_, mergeBase_);

			revisionTree_ = std::unique_ptr<GenericTree>(new GenericTree("MergeRevision", revision_));
			repository_->loadGenericTree(revisionTree_, revision_);

			headTree_ = std::unique_ptr<GenericTree>(new GenericTree("HeadTree", head_));
			repository_->loadGenericTree(headTree_, head_);

			buildConflictUnitMap(revisionCUToChangeMap_, revisionChangeToCUMap_, baseRevisionDiff_, revisionTree_,
										mergeBaseTree_);
			buildConflictUnitMap(headCUToChangeMap_, headChangeToCUMap_, baseHeadDiff_, headTree_, mergeBaseTree_);

			QList<Model::NodeIdType> conflicts = detectConflictingConflictUnits();
			stage_ = Stage::ConflictsDetected;

			if (!conflicts.isEmpty())
				markConflictRegions(conflicts);
				// TODO: MARK CONFLICT REGIONS


			computeMergeForLists(headTree_, revisionTree_, mergeBaseTree_, baseHeadDiff_.changes(), baseRevisionDiff_.changes());

			mergeTree_ = std::unique_ptr<GenericTree>(new GenericTree("Merge"));
			repository_->loadGenericTree(mergeTree_, head_);

			mergeChangesIntoTree(mergeTree_, baseRevisionDiff_.changes(), conflictRegions_);

			break;
		}

		default:
			Q_ASSERT(false);
	}
}

void Merge::performFastForward()
{
	QString branch = repository_->currentBranch();

	repository_->setReferenceTarget(branch, revision_);
	repository_->checkout(revision_, true);

	stage_ = Stage::Complete;
}

void Merge::buildConflictUnitMap(QMultiHash<Model::NodeIdType, ChangeDescription*>& cuToChange,
											QMultiHash<Model::NodeIdType, Model::NodeIdType>& changeToCU,
											const Diff& diff, const std::unique_ptr<GenericTree>& versionTree,
											const std::unique_ptr<GenericTree>& baseTree)
{
	for (ChangeDescription* change : diff.changes())
	{
		Model::NodeIdType conflictUnit;
		switch (change->type())
		{
			case ChangeType::Added:
				// find CU in treeB
				conflictUnit = findConflictUnit(change->nodeB(), versionTree, false, diff);
				insertIntoConflictUnitMaps(change, conflictUnit, cuToChange, changeToCU);
				break;

			case ChangeType::Deleted:
				// find CU in treeA
				conflictUnit = findConflictUnit(change->nodeA(), baseTree, true, diff);
				insertIntoConflictUnitMaps(change, conflictUnit, cuToChange, changeToCU);
				break;

			case ChangeType::Moved:
				// find CU in treeA
				conflictUnit = findConflictUnit(change->nodeA(), baseTree, true, diff);
				insertIntoConflictUnitMaps(change, conflictUnit, cuToChange, changeToCU);

				// find CU in treeB
				{
				Model::NodeIdType newConflictUnit = findConflictUnit(change->nodeB(), versionTree, false, diff);
				if (newConflictUnit != conflictUnit)
					insertIntoConflictUnitMaps(change, newConflictUnit, cuToChange, changeToCU);
				}
				break;

			case ChangeType::Stationary:
				if (change->flags().testFlag(ChangeDescription::Label))
				{
					// Reordering occured
					// find CU in treeA
					conflictUnit = findConflictUnit(change->nodeA(), baseTree, true, diff);
					insertIntoConflictUnitMaps(change, conflictUnit, cuToChange, changeToCU);

					// find CU in treeB
					{
					Model::NodeIdType newConflictUnit = findConflictUnit(change->nodeB(), versionTree, false, diff);
					if (newConflictUnit != conflictUnit)
						insertIntoConflictUnitMaps(change, newConflictUnit, cuToChange, changeToCU);
					}
				}
				else
				{
					conflictUnit = findConflictUnit(change->nodeB(), versionTree, false, diff);
					insertIntoConflictUnitMaps(change, conflictUnit, cuToChange, changeToCU);
				}
				break;

			default:
				Q_ASSERT(false);
		}
	}
}

void Merge::insertIntoConflictUnitMaps(ChangeDescription* change, Model::NodeIdType conflictUnit,
													QMultiHash<Model::NodeIdType, ChangeDescription*>& cuToChange,
													QMultiHash<Model::NodeIdType, Model::NodeIdType>& changeToCU)
{
	cuToChange.insertMulti(conflictUnit, change);
	changeToCU.insert(change->id(), conflictUnit);
}

Model::NodeIdType Merge::findConflictUnit(const GenericNode* node, const std::unique_ptr<GenericTree>& tree,
														bool inBase, const Diff& diff) const
{
	const GenericNode* nodeInTree = tree->find(node);
	IdToChangeDescriptionHash changes = diff.changes();

	while (nodeInTree)
	{
		if (isConflictUnitNode(nodeInTree))
		{
			if (inBase)
				return nodeInTree->id();

			IdToChangeDescriptionHash::iterator iter = changes.find(nodeInTree->id());
			if (iter == changes.end())
				return nodeInTree->id();
			else
			{
				ChangeDescription* change = iter.value();
				Q_ASSERT(!inBase);
				if (change->nodeA() && isConflictUnitNode(change->nodeA()))
					return nodeInTree->id();
			}
		}
		nodeInTree = nodeInTree->parent();
	}

	Q_ASSERT(false);
	return Model::NodeIdType();
}

// TODO: fill those lists correctly
const QStringList Merge::ORDERED_LISTS = {"StatementItemList",
														"TypedListOfResult",
														"TypedListOfFormalTypeArgument",
														"TypedListOfExpression",
														"TypedListOfDeclaration",
														"TypedListOfMemberInitializer",
														"TypedListOfEnumerator"
													  };

const QStringList Merge::UNORDERED_LISTS = {"TypedListOfClass",
														 "TypedListOfMethod",
														 "TypedListOfFormalArgument",
														 "TypedListOfFormalResult",
														 "TypedListOfField",
														 "TypedListOfUsedLibrary"
														 };


const QStringList Merge::STATEMENTS = {"Block",
													"BreakStatement",
													"CaseStatement",
													"ContinueStatement",
													"DeclarationStatement",
													"ExpressionStatement",
													"ForEachStatement",
													"IfStatement",
													"LoopStatement",
													"ReturnStatement",
													"Statement",
													"SwitchStatement",
													"TryCatchFinallyStatement"};

const QStringList Merge::DECLARATIONS = {"Class",
													  "Declaration",
													  "ExplicitTemplateInstantiation",
													  "Field",
													  "Method",
													  "Module",
													  "NameImport",
													  "Project",
													  "TypeAlias",
													  "VariableDeclaration"};

const QStringList Merge::ADDITIONAL_CONFLICT_UNIT_NODES = {"CommentStatementItem",
																			  "CatchClause",
																			  "StatementItemList"};

bool Merge::isConflictUnitNode(const GenericNode* node) const
{
	Q_ASSERT(node);

	for (QString nodeType : STATEMENTS)
		if (nodeType.compare(node->type()) == 0)
			return true;

	for (QString nodeType : DECLARATIONS)
		if (nodeType.compare(node->type()) == 0)
			return true;

	for (QString nodeType : ADDITIONAL_CONFLICT_UNIT_NODES)
		if (nodeType.compare(node->type()) == 0)
			return true;

	return false;
}

QList<Model::NodeIdType> Merge::detectConflictingConflictUnits()
{
	QList<Model::NodeIdType> conflictingCU;

	for (Model::NodeIdType id : headCUToChangeMap_.uniqueKeys())
	{
		int numChangesInHead = headCUToChangeMap_.count(id);
		int numChangesInRevision= revisionCUToChangeMap_.count(id);

		if (numChangesInHead > 0 && numChangesInRevision > 0)
			conflictingCU.append(id);
	}

	return conflictingCU;
}

void Merge::markConflictRegions(QList<Model::NodeIdType>& conflicts)
{
	for (auto conflict : conflicts)
	{
		// TODO
		(void) conflict;
	}
}

Merge::Chunk::Chunk(bool stable, QList<Model::NodeIdType> headList, QList<Model::NodeIdType> revisionList,
						QList<Model::NodeIdType> baseList)
	: stable_{stable}, head_{headList}, revision_{revisionList}, base_{baseList}
{}

void Merge::computeMergeForLists(const std::unique_ptr<GenericTree>& head, const std::unique_ptr<GenericTree>& revision,
											const std::unique_ptr<GenericTree>& base, const IdToChangeDescriptionHash& baseToHead,
											const IdToChangeDescriptionHash& baseToRevision)
{
	for (ChangeDescription* change : baseToRevision)
	{
		ChangeType type = change->type();
		if (type == ChangeType::Moved || type == ChangeType::Stationary)
		{
			ChangeDescription::UpdateFlags flags = change->flags();
			if (flags.testFlag(ChangeDescription::Children))
			{
				if (!nodeToRegionMap_.contains(change->id()))
				{
					GenericNode* revisionNode = revision->find(change->id(), change->nodeB()->persistentUnit()->name());
					GenericNode* baseNode = base->find(change->id(), change->nodeA()->persistentUnit()->name());

					GenericNode* headNode = baseNode;

					ChangeDescription* headChange = baseToHead.value(change->id());
					if (headChange && headChange->nodeB())
						headNode = head->find(change->id(), headChange->nodeB()->persistentUnit()->name());

					QList<Model::NodeIdType> headList = genericNodeListToNodeIdList(headNode->children());
					QList<Model::NodeIdType> revisionList = genericNodeListToNodeIdList(revisionNode->children());
					QList<Model::NodeIdType> baseList = genericNodeListToNodeIdList(baseNode->children());

					computeMergeChunks(headList, revisionList, baseList, change->id());
				}
			}
		}
	}
}

int Merge::listInsertionIndex(const QList<Model::NodeIdType>& target, const QList<Model::NodeIdType>& current,
										Model::NodeIdType insertID) const
{
	int targetIndex = target.indexOf(insertID);
	Q_ASSERT(targetIndex != -1);

	int index = -1;
	for (int i = targetIndex + 1; i < target.size(); ++i)
	{
		index = current.indexOf(target[i]);
		if (index >= 0)
			break;
	}

	if (index == -1)
		index = current.size();

	Q_ASSERT(0 <= index && index <= current.size());

	return index;
}


bool Merge::applyListMerge(QList<Model::NodeIdType>& mergedList, const QList<Chunk>& chunkList, bool resolveOrder) const
{
	for (Chunk chunk : chunkList)
	{
		if (chunk.stable_)
			mergedList.append(chunk.head_);
		else
		{
			if (chunk.base_ == chunk.head_ && chunk.base_ != chunk.revision_)
			{
				mergedList.append(chunk.revision_);
			}
			else if (chunk.base_ != chunk.head_ && chunk.base_ == chunk.revision_)
			{
				mergedList.append(chunk.head_);
			}
			else
			{
				if (!resolveOrder)
				{
					return false;
				}

				// Beware of inserting an ID twice
				QList<Model::NodeIdType> baseChunk;
				QList<Model::NodeIdType> headChunk(chunk.head_);
				QList<Model::NodeIdType> revisionChunk(chunk.revision_);

				for (Model::NodeIdType id : chunk.base_)
				{
					if (headChunk.contains(id) && revisionChunk.contains(id))
						baseChunk.append(id);
					else
					{
						headChunk.removeOne(id);
						revisionChunk.removeOne(id);
					}
				}

				QList<Model::NodeIdType> mergedChunk(headChunk);
				for (Model::NodeIdType id : revisionChunk)
				{
					if (!baseChunk.contains(id))
						mergedChunk.append(id);
				}

				mergedList.append(mergedChunk);
			}
		}
	}

	// every id is inserted only once
	Q_ASSERT(QSet<Model::NodeIdType>::fromList(mergedList).size() == mergedList.size());

	return true;
}

QList<Merge::Chunk>& Merge::computeMergeChunks(const QList<Model::NodeIdType> head,
															  const QList<Model::NodeIdType> revision,
															  const QList<Model::NodeIdType> base, Model::NodeIdType id)
{
	QList<Model::NodeIdType> lcsBaseToHead = longestCommonSubsequence(base, head);
	QList<Model::NodeIdType> lcsBaseToRevision = longestCommonSubsequence(base, revision);

	QSet<Model::NodeIdType> baseHead = QSet<Model::NodeIdType>::fromList(lcsBaseToHead);
	QSet<Model::NodeIdType> baseRevision = QSet<Model::NodeIdType>::fromList(lcsBaseToRevision);

	QList<Model::NodeIdType> stableIDs;
	for (Model::NodeIdType id : base)
		if (baseHead.contains(id) && baseRevision.contains(id))
			stableIDs.append(id);

	QList<QList<Model::NodeIdType>> sublistHead = computeSublists(head, stableIDs);
	QList<QList<Model::NodeIdType>> sublistRevision = computeSublists(revision, stableIDs);
	QList<QList<Model::NodeIdType>> sublistBase = computeSublists(base, stableIDs);

	QList<Chunk> chunks;

	QList<QList<Model::NodeIdType>>::const_iterator iterA = sublistHead.constBegin();
	QList<QList<Model::NodeIdType>>::const_iterator iterB = sublistRevision.constBegin();
	QList<QList<Model::NodeIdType>>::const_iterator iterBase;

	bool isStable = false;
	for (iterBase = sublistBase.constBegin(); iterBase != sublistBase.constEnd(); ++iterBase)
	{
		if (!iterBase->isEmpty() || !iterA->isEmpty() || !iterB->isEmpty())
			chunks.append(Chunk(isStable, *iterA, *iterB, *iterBase));
		isStable = !isStable;
		++iterA;
		++iterB;
	}
	Q_ASSERT(isStable);

	auto iterator = mergedLists_.insert(id, chunks);

	return iterator.value();
}

QList<QList<Model::NodeIdType>> Merge::computeSublists(const QList<Model::NodeIdType> list,
																		 const QList<Model::NodeIdType> stableIDs)
{
	QList<QList<Model::NodeIdType>> chunks;
	QList<Model::NodeIdType> chunk;

	QList<Model::NodeIdType>::const_iterator stableId = stableIDs.begin();
	for (Model::NodeIdType id : list)
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

QList<Model::NodeIdType> Merge::longestCommonSubsequence(const QList<Model::NodeIdType> listA,
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

QList<Model::NodeIdType> Merge::backtrackLCS(int** data, const QList<Model::NodeIdType> listA,
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

QList<Model::NodeIdType> Merge::genericNodeListToNodeIdList(const QList<GenericNode*>& list)
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


Merge::ListType Merge::getListType(const GenericNode* node)
{
	for (QString listType : ORDERED_LISTS)
		if (listType.compare(node->type()) == 0)
			return ListType::OrderedList;

	for (QString listType : UNORDERED_LISTS)
		if (listType.compare(node->type()) == 0)
			return ListType::UnorderedList;

	return ListType::NoList;
}

void Merge::mergeChangesIntoTree(const std::unique_ptr<GenericTree>& tree, const IdToChangeDescriptionHash& changes,
											QList<QSet<Model::NodeIdType>>& conflictRegions)
{
	IdToChangeDescriptionHash applicableChanges;
	for (ChangeDescription* change : changes.values())
	{
		bool doesConflict = false;
		for (QSet<Model::NodeIdType> conflictRegion : conflictRegions)
			if (conflictRegion.contains(change->id()))
				doesConflict = true;

		if (!doesConflict)
			applicableChanges.insert(change->id(), change);
	}

	applyChangesToTree(tree, applicableChanges);
}

void Merge::applyChangesToTree(const std::unique_ptr<GenericTree>& tree, const IdToChangeDescriptionHash& changes)
{
	QList<ChangeDescription*> changeQueue = changes.values();
	IdToChangeDescriptionHash unappliedChanges(changes);

	while (!changeQueue.isEmpty())
	{
		ChangeDescription* change = changeQueue.takeFirst();
		bool success = false;

		switch (change->type())
		{
			case ChangeType::Added:
				success = applyAddToTree(tree, unappliedChanges, change);
				break;

			case ChangeType::Deleted:
				success = applyDeleteToTree(tree, unappliedChanges, change);
				break;

			case ChangeType::Moved:
				success = applyMoveToTree(tree, unappliedChanges, change);
				break;

			case ChangeType::Stationary:
				success = applyStationaryChangeToTree(tree, unappliedChanges, change);
				break;

			default:
				Q_ASSERT(false);
		}

		if (success)
			unappliedChanges.remove(change->id());
		else
			changeQueue.append(change);
	}
}

bool Merge::applyAddToTree(const std::unique_ptr<GenericTree>& tree, IdToChangeDescriptionHash& changes,
									const ChangeDescription* addOp)
{
	GenericNode* parent = addOp->nodeB()->parent();

	if (parent && changes.contains(parent->id()))
		return false;

	QString persistentUnitName = addOp->nodeB()->persistentUnit()->name();
	GenericPersistentUnit* persistentUnit = tree->persistentUnit(persistentUnitName);
	if (!persistentUnit)
		persistentUnit = &tree->newPersistentUnit(persistentUnitName);

	// perform insert
	GenericNode* newNode = persistentUnit->newNode(addOp->nodeB());
	Q_ASSERT(!newNode->parent());
	Q_ASSERT(newNode->children().isEmpty());

	if (parent)
	{
		parent = tree->find(parent);

		if (isListType(parent))
			performInsertIntoList(parent, newNode);
		else
		{
			parent->addChild(newNode);
			newNode->setParent(parent);
		}
	}

	return true;
}

bool Merge::applyDeleteToTree(const std::unique_ptr<GenericTree>& tree, IdToChangeDescriptionHash& /*changes*/,
										const ChangeDescription* deleteOp)
{
	GenericNode* node = tree->find(deleteOp->nodeA());
	Q_ASSERT(node);

	if (node->children().size() > 0)
		return false;

	GenericNode* parent = node->parent();
	if (parent && isListType(parent))
	{
		int index = node->name().toInt();
		for (GenericNode* child : parent->children())
		{
			int childIndex = child->name().toInt();
			if (index < childIndex)
			{
				--childIndex;
				child->setName(QString::number(childIndex));
			}
		}
	}
	node->remove();

	return true;
}

bool Merge::applyMoveToTree(const std::unique_ptr<GenericTree>& tree, IdToChangeDescriptionHash& changes,
									 const ChangeDescription* moveOp)
{
	GenericNode* targetParent = moveOp->nodeB()->parent();
	if (targetParent && changes.contains(targetParent->id()))
		return false;

	QString targetPersistentUnitName = moveOp->nodeB()->persistentUnit()->name();
	GenericPersistentUnit* targetPersistentUnit = tree->persistentUnit(targetPersistentUnitName);
	if (!targetPersistentUnit)
		targetPersistentUnit = &tree->newPersistentUnit(targetPersistentUnitName);

	GenericNode* nodeToMove = tree->find(moveOp->nodeA());
	Q_ASSERT(nodeToMove);
	GenericNode* sourceParent = nodeToMove->parent();

	GenericNode* newNode = targetPersistentUnit->newNode(nodeToMove, true);
	nodeToMove->detach();

	if (sourceParent && isListType(sourceParent))
	{
		int index = nodeToMove->name().toInt();
		for (GenericNode* child : sourceParent->children())
		{
			int childIndex = child->name().toInt();
			if (index < childIndex)
			{
				--childIndex;
				child->setName(QString::number(childIndex));
			}
		}
	}

	if (targetParent)
	{
		targetParent = tree->find(targetParent->id(), targetParent->persistentUnit()->name());

		if (isListType(targetParent))
			performInsertIntoList(targetParent, newNode);
		else
		{
			targetParent->addChild(newNode);
			newNode->setParent(targetParent);
		}
	}

	return true;
}

bool Merge::applyStationaryChangeToTree(const std::unique_ptr<GenericTree>& tree,
													 IdToChangeDescriptionHash& /*changes*/,
													 const ChangeDescription* stationaryOp)
{
	GenericNode* node = tree->find(stationaryOp->nodeA());
	Q_ASSERT(node);

	ChangeDescription::UpdateFlags flags = stationaryOp->flags();

	if (flags.testFlag(ChangeDescription::Value))
	{
		GenericNode::ValueType type = stationaryOp->nodeB()->valueType();
		QString value = stationaryOp->nodeB()->rawValue();
		node->resetValue(type, value);
	}

	if (flags.testFlag(ChangeDescription::Type))
	{
		QString type = stationaryOp->nodeB()->type();
		node->setType(type);
	}

	if (flags.testFlag(ChangeDescription::Label))
	{
		GenericNode* parent = stationaryOp->nodeB()->parent();
		if (parent)
		{
			parent = tree->find(parent);
			Q_ASSERT(parent);

			if (isListType(parent))
			{
				if (!reorderedLists_.contains(parent->id()))
					performReorderInList(parent, node);
			}
			else
				node->setName(stationaryOp->nodeB()->name());
		}
		else
			node->setName(stationaryOp->nodeB()->name());
	}

	return true;
}

void Merge::performInsertIntoList(GenericNode* parent, GenericNode* node)
{
	if (mergedLists_.contains(parent->id()))
	{
		ListType listType = getListType(parent);
		bool resolveOrder = listType != ListType::OrderedList;

		QList<Model::NodeIdType> targetList;
		bool success = applyListMerge(targetList, mergedLists_.value(parent->id()), resolveOrder);
		Q_ASSERT(success);
		int index = listInsertionIndex(targetList, genericNodeListToNodeIdList(parent->children()), node->id());

		for (auto child : parent->children())
		{
			int childIndex = child->name().toInt();
			if (index <= childIndex)
			{
				++childIndex;
				child->setName(QString::number(childIndex));
			}
		}

		node->setName(QString::number(index));
		parent->addChild(node);
	}
	else
	{
		parent->addChild(node);
		node->setParent(parent);
	}
}

void Merge::performReorderInList(GenericNode* parent, GenericNode* node)
{
	Q_ASSERT(mergedLists_.contains(parent->id()));
	Q_ASSERT(node->parent() == parent);

	ListType listType = getListType(parent);
	bool resolveOrder = listType != ListType::OrderedList;

	QList<Model::NodeIdType> targetList;
	bool success = applyListMerge(targetList, mergedLists_.value(parent->id()), resolveOrder);
	Q_ASSERT(success);

	QSet<Model::NodeIdType> availableIds =
			QSet<Model::NodeIdType>::fromList(genericNodeListToNodeIdList(parent->children()));

	QHash<Model::NodeIdType, QString> indexList;
	int index = 0;
	for (auto id : targetList)
	{
		if (availableIds.contains(id))
		{
			indexList.insert(id, QString::number(index));
			++index;
		}
	}

	for (auto child : parent->children())
	{
		if (indexList.contains(child->id()))
			child->setName(indexList.value(child->id()));
		else
		{
			// these nodes will be deleted but still need a valid temporary index
			child->setName(QString::number(index));
			++index;
		}
	}

	reorderedLists_.insert(parent->id());
}

} /* namespace FilePersistence */
