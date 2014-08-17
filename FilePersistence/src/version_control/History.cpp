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

#include "History.h"

#include "GitRepository.h"
#include "ChangeDescription.h"

#include "../simple/Parser.h"

namespace FilePersistence {

History::CommitTime::CommitTime(QString sha1, qint64 time)
{
	commitSHA1_ = sha1;
	timeSinceEpoch_ = time;
}

bool History::CommitTime::earlier(const CommitTime& left, const CommitTime& right)
{
	return (left.timeSinceEpoch_ < right.timeSinceEpoch_);
}

bool History::CommitTime::later(const CommitTime& left, const CommitTime& right)
{
	return (left.timeSinceEpoch_ > right.timeSinceEpoch_);
}

History::History(QString relativePath, Model::NodeIdType rootNodeId,
					  const CommitGraph* historyGraph, const GitRepository* repository)
{
	rootNodeId_ = rootNodeId;
	historyGraph_ = historyGraph;

	const CommitGraphItem endItem = historyGraph_->end();
	QSet<const CommitGraphItem*> visited;
	SHA1 end = endItem.commitSHA1_;
	QSet<Model::NodeIdType> trackedIDs = trackSubtree(end, relativePath, repository);

	detectRelevantCommits(&endItem, visited, relativePath, trackedIDs, repository);
}

QList<QString> History::relevantCommitsByTime(const GitRepository* repository, bool reverse) const
{
	QList<CommitTime> commitTimeList;
	for (auto commit : relevantCommits_)
	{
		CommitMetaData info = repository->getCommitInformation(commit);
		qint64 time = info.dateTime_.toMSecsSinceEpoch();
		commitTimeList.append(CommitTime(commit, time));
	}

	if (reverse)
		qSort(commitTimeList.begin(), commitTimeList.end(), CommitTime::later);
	else
		qSort(commitTimeList.begin(), commitTimeList.end(), CommitTime::earlier);

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
			{
				if (trackedIDs.contains(change->id()))
				{
					subtreeIsAffected = true;
					break;
				}
			}

			if (!subtreeIsAffected)
				isRelevant = false;

			QString parentRelativeRootPath =findRootPath(relativePathRootNode, &diff);

			if (!parentRelativeRootPath.isNull())
			{
				if (subtreeIsAffected)
				{
					QSet<Model::NodeIdType> newTrackedIDs = trackSubtree(parent->commitSHA1_, parentRelativeRootPath,
																						  repository);

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

QString History::findRootPath(QString currentPath, const Diff* diff)
{
	IdToChangeDescriptionHash changes = diff->changes();

	// check if rootNode is affected by changes directly
	IdToChangeDescriptionHash::iterator iter = changes.find(rootNodeId_);
	if (iter != changes.end())
	{
		ChangeDescription* change = iter.value();
		const GenericNode* rootNode = change->oldNode();
		if (rootNode)
			return rootNode->persistentUnit()->name();
		else
			return QString();
	}

	// check if rootNode is still in current PU
	// TODO find rootNodeId in PU

	// check if rootNode is in other PU (affected by move)
	IdToChangeDescriptionHash moves = diff->changes(ChangeType::Moved);
	for (auto move : moves)
	{
		if (currentPath.compare(move->newNode()->persistentUnit()->name()) == 0)
		{
			// TODO find rootNodeId in PU
		}
	}

	// TODO return correct path
	return currentPath;
}

QSet<Model::NodeIdType> History::trackSubtree(QString revision, QString relativePath,
															 const GitRepository* repository) const
{
	QSet<Model::NodeIdType> trackedIDs;
	QList<const CommitFile*> commitFiles;


	const CommitFile* startFile = repository->getCommitFile(revision, relativePath);
	commitFiles.append(startFile);

	GenericTree tree("History", revision);

	GenericPersistentUnit unit = tree.newPersistentUnit(relativePath);
	GenericNode* unitRoot = Parser::load(startFile->content_, startFile->size_, false, unit);

	GenericNode* subtreeRoot = unitRoot->find(rootNodeId_);
	if (!subtreeRoot)
		return QSet<Model::NodeIdType>();

	Q_ASSERT(subtreeRoot != nullptr);

	QList<GenericNode*> stack;
	stack.append(subtreeRoot);
	while (!stack.empty())
	{
		GenericNode* current = stack.last();
		stack.removeLast();

		trackedIDs.insert(current->id());

		if (persistenceUnitType.compare(current->type()) == 0)
		{
			QString subUnitrelativePath = current->id().toString();
			const CommitFile* file = repository->getCommitFile(revision, subUnitrelativePath);
			commitFiles.append(file);
			GenericPersistentUnit subUnit = tree.newPersistentUnit(subUnitrelativePath);
			GenericNode* subUnitRoot = Parser::load(file->content_, file->size_, false, subUnit);
			stack.append(subUnitRoot);
		}

		for (auto child : current->children())
			stack.append(child);
	}

	for (auto file : commitFiles)
		SAFE_DELETE(file);

	return trackedIDs;
}

const QString History::persistenceUnitType = "persistencenewunit";

} /* namespace FilePersistence */
