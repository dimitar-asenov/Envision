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

#include "FilePersistence/src/simple/Parser.h"
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
			loadGenericTree(mergeBaseTree_, mergeBase_);

			revisionTree_ = std::unique_ptr<GenericTree>(new GenericTree("MergeRevision", revision_));
			loadGenericTree(revisionTree_, revision_);

			headTree_ = std::unique_ptr<GenericTree>(new GenericTree("HeadTree", head_));
			loadGenericTree(headTree_, head_);

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
			loadGenericTree(mergeTree_, head_);

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

void Merge::buildConflictUnitMap(IdToChangeDescriptionHash& cuToChange,
											QHash<Model::NodeIdType, Model::NodeIdType>& changeToCU,
											const Diff& diff, const std::unique_ptr<GenericTree>& versionTree,
											const std::unique_ptr<GenericTree>& baseTree)
{
	for (ChangeDescription* change : diff.changes())
	{
		Model::NodeIdType conflictUnit;
		switch (change->type())
		{
			case ChangeType::Added:
			{
				// find CU in treeB
				QString unitName = change->nodeB()->persistentUnit()->name();
				const GenericPersistentUnit* unit = versionTree->persistentUnit(unitName);
				Q_ASSERT(unit);
				conflictUnit = findConflicUnit(change->id(), false, diff, unit);
				cuToChange.insertMulti(conflictUnit, change);
				changeToCU.insert(change->id(), conflictUnit);
				break;
			}

			case ChangeType::Deleted:
			{
				// find CU in treeA
				QString unitName = change->nodeA()->persistentUnit()->name();
				const GenericPersistentUnit* unit = baseTree->persistentUnit(unitName);
				Q_ASSERT(unit);
				conflictUnit = findConflicUnit(change->id(), true, diff, unit);
				cuToChange.insertMulti(conflictUnit, change);
				changeToCU.insert(change->id(), conflictUnit);
				break;
			}

			case ChangeType::Moved:
			{
				// find CU in treeA
				QString unitName = change->nodeA()->persistentUnit()->name();
				const GenericPersistentUnit* unit = baseTree->persistentUnit(unitName);
				Q_ASSERT(unit);
				conflictUnit = findConflicUnit(change->id(), true, diff, unit);
				cuToChange.insertMulti(conflictUnit, change);
				changeToCU.insertMulti(change->id(), conflictUnit);

				// find CU in treeB
				unitName = change->nodeB()->persistentUnit()->name();
				unit = versionTree->persistentUnit(unitName);
				Q_ASSERT(unit);
				Model::NodeIdType newConflictUnit = findConflicUnit(change->id(), false, diff, unit);
				if (newConflictUnit != conflictUnit)
				{
					cuToChange.insertMulti(newConflictUnit, change);
					changeToCU.insertMulti(change->id(), newConflictUnit);
				}
				break;
			}

			case ChangeType::Stationary:
			{
				if (change->flags().testFlag(ChangeDescription::Order))
				{
					// Reordering occured
					// find CU in treeA
					QString unitName = change->nodeA()->persistentUnit()->name();
					const GenericPersistentUnit* unit = baseTree->persistentUnit(unitName);
					Q_ASSERT(unit);
					conflictUnit = findConflicUnit(change->id(), true, diff, unit);
					cuToChange.insertMulti(conflictUnit, change);
					changeToCU.insertMulti(change->id(), conflictUnit);

					// find CU in treeB
					unitName = change->nodeB()->persistentUnit()->name();
					unit = versionTree->persistentUnit(unitName);
					Q_ASSERT(unit);
					Model::NodeIdType newConflictUnit = findConflicUnit(change->id(), false, diff, unit);
					if (newConflictUnit != conflictUnit)
					{
						cuToChange.insertMulti(newConflictUnit, change);
						changeToCU.insertMulti(change->id(), newConflictUnit);
					}
				}
				else
				{
					QString unitName = change->nodeB()->persistentUnit()->name();
					const GenericPersistentUnit* unit = versionTree->persistentUnit(unitName);
					Q_ASSERT(unit);
					conflictUnit = findConflicUnit(change->id(), false, diff, unit);
					cuToChange.insertMulti(conflictUnit, change);
					changeToCU.insert(change->id(), conflictUnit);
					break;
				}
				break;
			}

			default:
				Q_ASSERT(false);
		}
	}
}

Model::NodeIdType Merge::findConflicUnit(Model::NodeIdType nodeID, bool inBase, const Diff& diff,
													  const GenericPersistentUnit* unit) const
{
	const GenericNode* node = unit->find(nodeID);
	IdToChangeDescriptionHash changes = diff.changes();

	while (node)
	{
		if (isConflictUnitNode(node))
		{
			if (inBase)
				return node->id();

			IdToChangeDescriptionHash::iterator iter = changes.find(node->id());
			if (iter == changes.end())
				return node->id();
			else
			{
				ChangeDescription* change = iter.value();
				Q_ASSERT(!inBase);
				if (change->nodeA() && isConflictUnitNode(change->nodeA()))
					return node->id();
			}
		}
		node = node->parent();
	}

	Q_ASSERT(false);
	return nodeID;
}

// TODO: fill those lists correctly
const QStringList Merge::ORDERED_LISTS = {"StatementItemList"
													  };

const QStringList Merge::UNORDERED_LISTS = {"TypedListOfClass",
														 "TypedListOfMethod",
														 "TypedListOfFormalArgument",
														 "TypedListOfField"};


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

const QStringList Merge::ADDITIONAL_NODES = {"CommentStatementItem",
															"CatchClause",
															"StatementItemList"};

bool Merge::isConflictUnit(const GenericNode* node, NodeSource source, const ChangeDescription* baseToSource) const
{
	if (isConflictUnitNode(node))
	{
		if (source == NodeSource::Base)
			return true;

		if (!baseToSource)
			return true;

		if (baseToSource->nodeA() && isConflictUnitNode(baseToSource->nodeA()))
			return true;
	}

	return false;
}

bool Merge::isConflictUnitNode(const GenericNode* node) const
{
	Q_ASSERT(node);

	for (QString nodeType : STATEMENTS)
		if (nodeType.compare(node->type()) == 0)
			return true;

	for (QString nodeType : DECLARATIONS)
		if (nodeType.compare(node->type()) == 0)
			return true;

	for (QString nodeType : ADDITIONAL_NODES)
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

Merge::Hunk::Hunk(bool stable, QList<Model::NodeIdType> headList, QList<Model::NodeIdType> revisionList,
						QList<Model::NodeIdType> baseList)
{
	stable_ = stable;
	head_ = headList;
	revision_ = revisionList;
	base_ = baseList;
}

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

					mergeLists(headList, revisionList, baseList, change->id());
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


QList<Model::NodeIdType> Merge::applyListMerge(const QList<Hunk>& hunkList, bool resolveOrder) const
{
	QList<Model::NodeIdType> mergedList;

	for (Hunk hunk : hunkList)
	{
		if (hunk.stable_)
			mergedList.append(hunk.head_);
		else
		{
			if (hunk.base_ == hunk.head_ && hunk.base_ != hunk.revision_)
			{
				mergedList.append(hunk.revision_);
			}
			else if (hunk.base_ != hunk.head_ && hunk.base_ == hunk.revision_)
			{
				mergedList.append(hunk.head_);
			}
			else
			{
				if (!resolveOrder)
				{
					return QList<Model::NodeIdType>();
				}

				// Beware of inserting an ID twice
				QList<Model::NodeIdType> baseHunk;
				QList<Model::NodeIdType> headHunk(hunk.head_);
				QList<Model::NodeIdType> revisionHunk(hunk.revision_);

				for (Model::NodeIdType id : hunk.base_)
				{
					if (headHunk.contains(id) && revisionHunk.contains(id))
						baseHunk.append(id);
					else
					{
						headHunk.removeOne(id);
						revisionHunk.removeOne(id);
					}
				}

				QList<Model::NodeIdType> mergedHunk(headHunk);
				for (Model::NodeIdType id : revisionHunk)
				{
					if (!baseHunk.contains(id))
						mergedHunk.append(id);
				}

				mergedList.append(mergedHunk);
			}
		}
	}

	return mergedList;
}

QList<Merge::Hunk>& Merge::mergeLists(const QList<Model::NodeIdType> head, const QList<Model::NodeIdType> revision,
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

	QList<Hunk> hunks;

	QList<QList<Model::NodeIdType>>::const_iterator iterA = sublistHead.constBegin();
	QList<QList<Model::NodeIdType>>::const_iterator iterB = sublistRevision.constBegin();
	QList<QList<Model::NodeIdType>>::const_iterator iterBase;

	bool isStable = false;
	for (iterBase = sublistBase.constBegin(); iterBase != sublistBase.constEnd(); ++iterBase)
	{
		if (!iterBase->isEmpty() || !iterA->isEmpty() || !iterB->isEmpty())
			hunks.append(Hunk(isStable, *iterA, *iterB, *iterBase));
		isStable = !isStable;
		++iterA;
		++iterB;
	}
	Q_ASSERT(isStable);

	auto iterator = mergedLists_.insert(id, hunks);

	return iterator.value();
}

QList<QList<Model::NodeIdType>> Merge::computeSublists(const QList<Model::NodeIdType> list,
																		 const QList<Model::NodeIdType> stableIDs)
{
	QList<QList<Model::NodeIdType>> hunks;
	QList<Model::NodeIdType> hunk;

	QList<Model::NodeIdType>::const_iterator stableId = stableIDs.begin();
	for (Model::NodeIdType id : list)
	{
		if (stableId != stableIDs.constEnd() && id == *stableId)
		{
			hunks.append(hunk);
			hunk = QList<Model::NodeIdType>();
			hunk.append(id);
			hunks.append(hunk);
			hunk = QList<Model::NodeIdType>();
			stableId++;
		}
		else
			hunk.append(id);
	}
	hunks.append(hunk);

	Q_ASSERT(hunks.size() == 2*stableIDs.size() + 1);

	return hunks;
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

bool Merge::isListType(const GenericNode* node)
{
	if (getListType(node) == ListType::NoList)
		return false;
	else
		return true;
}

void Merge::loadGenericTree(const std::unique_ptr<GenericTree>& tree, const QString version)
{
	IdToGenericNodeHash persistentUnitRoots;

	const Commit* commit = repository_->getCommit(version);
	for (auto file : commit->files())
	{
		GenericNode* unitRoot = Parser::load(file->content(), file->size_, false,
														 tree->newPersistentUnit(file->relativePath_));

		Q_ASSERT(unitRoot);
		persistentUnitRoots.insert(unitRoot->id(), unitRoot);
		Q_ASSERT(isConflictUnitNode(unitRoot));
	}

	IdToGenericNodeHash persistentUnitDeclarations;
	for (GenericNode* node : persistentUnitRoots)
		findPersistentUnitDeclarations(node, persistentUnitDeclarations);

	for (GenericNode* root : persistentUnitRoots)
	{
		GenericNode* declaration = persistentUnitDeclarations.value(root->id());

		if (declaration)
		{
			GenericNode* parent = declaration->parent();
			root->setParent(parent);
			parent->addChild(root);

			declaration->remove();
		}
	}

	SAFE_DELETE(commit);
}

void Merge::findPersistentUnitDeclarations(GenericNode* node, IdToGenericNodeHash& declarations)
{
	if (node->type().compare(GenericNode::persistentUnitType) == 0)
		declarations.insert(node->id(), node);
	else
		for (GenericNode* child : node->children())
			findPersistentUnitDeclarations(child, declarations);
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
		parent = tree->find(parent->id(), parent->persistentUnit()->name());

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
	QString persistentUnitName = deleteOp->nodeA()->persistentUnit()->name();
	GenericNode* node = tree->find(deleteOp->id(), persistentUnitName);
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

	GenericNode* nodeToMove = tree->find(moveOp->id(), moveOp->nodeA()->persistentUnit()->name());
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
	GenericNode* node = tree->find(stationaryOp->id(), stationaryOp->nodeA()->persistentUnit()->name());
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

	if (flags.testFlag(ChangeDescription::Order))
	{
		GenericNode* parent = stationaryOp->nodeB()->parent();
		if (parent)
		{
			parent = tree->find(parent->id(), parent->persistentUnit()->name());
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
	Q_ASSERT(mergedLists_.contains(parent->id()));

	ListType listType = getListType(parent);
	bool resolveOrder;
	if (listType == ListType::OrderedList)
		resolveOrder = false;
	else
		resolveOrder = true;

	QList<Model::NodeIdType> targetList = applyListMerge(mergedLists_.value(parent->id()), resolveOrder);
	Q_ASSERT(!targetList.empty());
	int index = listInsertionIndex(targetList, genericNodeListToNodeIdList(parent->children()), node->id());

	for (GenericNode* child : parent->children())
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

void Merge::performReorderInList(GenericNode* parent, GenericNode* node)
{
	Q_ASSERT(mergedLists_.contains(parent->id()));
	Q_ASSERT(node->parent() == parent);

	ListType listType = getListType(parent);
	bool resolveOrder;
	if (listType == ListType::OrderedList)
		resolveOrder = false;
	else
		resolveOrder = true;

	QList<Model::NodeIdType> targetList = applyListMerge(mergedLists_.value(parent->id()), resolveOrder);
	Q_ASSERT(!targetList.empty());

	QSet<Model::NodeIdType> availableIds =
			QSet<Model::NodeIdType>::fromList(genericNodeListToNodeIdList(parent->children()));

	QHash<Model::NodeIdType, QString> indexList;
	int index = 0;
	for (Model::NodeIdType id : targetList)
	{
		if (availableIds.contains(id))
		{
			indexList.insert(id, QString::number(index));
			qDebug() << id << index;
			++index;
		}
	}

	for (GenericNode* child : parent->children())
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
