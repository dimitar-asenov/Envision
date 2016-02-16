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

#include "History.h"

#include "GitRepository.h"
#include "ChangeDescription.h"

#include "../simple/Parser.h"

namespace FilePersistence {

History::CommitTime::CommitTime(QString sha1, QDateTime dateTime)
	:commitSHA1_{sha1}, dateTime_{dateTime}
{}

bool History::CommitTime::operator < (const CommitTime& commitTime) const
{
	return dateTime_ < commitTime.dateTime_;
}

bool History::CommitTime::operator > (const CommitTime& commitTime) const
{
	return dateTime_ > commitTime.dateTime_;
}

bool History::CommitTime::later(const CommitTime& left, const CommitTime& right)
{
	return left > right;
}

History::History(QString relativePath, Model::NodeIdType rootNodeId,
					  const CommitGraph* historyGraph, const GitRepository* repository)
{
	rootNodeId_ = rootNodeId;
	historyGraph_ = historyGraph;

	const CommitGraphItem endItem = historyGraph_->end();
	QSet<const CommitGraphItem*> visited;
	QString end = endItem.commitSHA1_;

	GenericTree initialTree{"History"};
	QSet<Model::NodeIdType> trackedIDs = trackSubtree(end, relativePath, &initialTree, repository);

	detectRelevantCommits(&endItem, visited, relativePath, trackedIDs, repository);
}

QList<QString> History::relevantCommitsByTime(const GitRepository* repository, bool reverse) const
{
	QList<CommitTime> commitTimeList;
	for (auto commit : relevantCommits_)
	{
		CommitMetaData info = repository->getCommitInformation(commit);
		commitTimeList.append(CommitTime{commit, info.dateTime_});
	}

	if (reverse)
		std::sort(commitTimeList.begin(), commitTimeList.end(), CommitTime::later);
	else
		std::sort(commitTimeList.begin(), commitTimeList.end());

	QList<QString> orderedCommits;
	for (auto commitTime : commitTimeList)
		orderedCommits.append(commitTime.commitSHA1_);

	return orderedCommits;
}

void History::detectRelevantCommits(const CommitGraphItem* current, QSet<const CommitGraphItem*> visited,
												QString relativePathRootNode, QSet<Model::NodeIdType> trackedIDs,
												const GitRepository* repository)
{
	if (!visited.contains(current) && !trackedIDs.isEmpty())
	{
		visited.insert(current);
		bool isRelevant = true;
		for (auto parent : current->parents_)
		{
			Diff diff = repository->diff(parent->commitSHA1_, current->commitSHA1_);
			IdToChangeDescriptionHash changes = diff.changes();

			bool subtreeIsAffected = false;
			for (auto change : changes.values())
				if (trackedIDs.contains(change->nodeId()))
				{
					subtreeIsAffected = true;
					break;
				}

			if (!subtreeIsAffected)
				isRelevant = false;

			GenericTree tree{"History"};
			QString parentRelativeRootPath =findRootPath(parent->commitSHA1_, relativePathRootNode, &diff,
																		&tree, repository);

			if (!parentRelativeRootPath.isNull())
			{
				if (subtreeIsAffected)
				{
					QSet<Model::NodeIdType> newTrackedIDs = trackSubtree(parent->commitSHA1_, parentRelativeRootPath,
																						  &tree, repository);

					detectRelevantCommits(parent, visited, parentRelativeRootPath, newTrackedIDs, repository);
				}
				else
					detectRelevantCommits(parent, visited, parentRelativeRootPath, trackedIDs, repository);
			}
		}

		if (isRelevant)
			relevantCommits_.insert(current->commitSHA1_);
	}
}

QString History::findRootPath(QString revision, QString currentPath, const Diff* diff, GenericTree* tree,
										const GitRepository* repository)
{
	IdToChangeDescriptionHash changes = diff->changes();

	// check if rootNode is affected by changes directly
	if (changes.contains(rootNodeId_))
	{
		auto change = changes.value(rootNodeId_);
		const GenericNode* rootNode = change->nodeA();
		if (rootNode)
			return rootNode->persistentUnit()->name();
		else
			return QString{};
	}

	// check if rootNode is still in current PU
	if (!tree->persistentUnit(currentPath)) {
		const CommitFile* file = repository->getCommitFile(revision, currentPath);
		Parser::load(file->content(), file->size_, false, tree->newPersistentUnit(currentPath));
		SAFE_DELETE(file);
	}

	tree->buildLookupHash();

	GenericNode* rootNode = tree->find(rootNodeId_);
	if (rootNode)
		return currentPath;

	// check if rootNode is in other PU (affected by move)
	QSet<QString> alreadyChecked;
	alreadyChecked.insert(currentPath);
	IdToChangeDescriptionHash moves = diff->changes(ChangeType::Move);
	for (auto move : moves)
		if (currentPath.compare(move->nodeB()->persistentUnit()->name()) == 0)
		{
			QString unitName = move->nodeA()->persistentUnit()->name();
			if (!alreadyChecked.contains(unitName))
			{
				if (!tree->persistentUnit(unitName))
				{
					const CommitFile* file = repository->getCommitFile(revision, unitName);
					Parser::load(file->content(), file->size_, false, tree->newPersistentUnit(unitName));
					SAFE_DELETE(file);
				}
				GenericNode* rootNode = tree->find(rootNodeId_);
				if (rootNode)
					return unitName;
			}
		}

	// rootNodeId can't be found but was not deleted!
	Q_ASSERT(false);
	return QString{};
}

QSet<Model::NodeIdType> History::trackSubtree(QString revision, QString relativePath, GenericTree* tree,
															 const GitRepository* repository) const
{
	QSet<Model::NodeIdType> trackedIDs;
	QList<const CommitFile*> commitFiles;


	if (!tree->persistentUnit(relativePath))
	{
		const CommitFile* startFile = repository->getCommitFile(revision, relativePath);
		commitFiles.append(startFile);

		Parser::load(startFile->content(), startFile->size_, false, tree->newPersistentUnit(relativePath));
	}

	tree->buildLookupHash();
	GenericNode* subtreeRoot = tree->find(rootNodeId_);
	if (!subtreeRoot)
		return QSet<Model::NodeIdType>{};

	Q_ASSERT(subtreeRoot != nullptr);

	QList<GenericNode*> stack;
	stack.append(subtreeRoot);
	while (!stack.empty())
	{
		GenericNode* current = stack.last();
		stack.removeLast();

		trackedIDs.insert(current->id());

		if (current->type() == GenericNode::PERSISTENT_UNIT_TYPE)
		{
			QString subUnitrelativePath = current->id().toString();

			if (!tree->persistentUnit(subUnitrelativePath))
			{
				const CommitFile* file = repository->getCommitFile(revision, subUnitrelativePath);
				commitFiles.append(file);

				Parser::load(file->content(), file->size_, false, tree->newPersistentUnit(subUnitrelativePath));
			}
			GenericNode* subUnitRoot = tree->persistentUnit(subUnitrelativePath)->unitRootNode();
			stack.append(subUnitRoot);
		}

		for (auto child : current->children())
			stack.append(child);
	}

	for (auto file : commitFiles)
		SAFE_DELETE(file);

	return trackedIDs;
}

}
