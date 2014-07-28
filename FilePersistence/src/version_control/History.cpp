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

History::History(QString relativePath, Model::NodeIdType rootNodeId,
					  const CommitGraph* historyGraph, const GitRepository* repository)
{
	rootNodeId_ = rootNodeId;
	historyGraph_ = historyGraph;

	const CommitGraphItem start = historyGraph_->start();
	relevantCommits_.insert(start.commitSHA1_);

	QString startRevision = historyGraph_->start().commitSHA1_;
	QSet<Model::NodeIdType> trackedIDs = trackSubtree(startRevision, relativePath, repository);
	detectRelevantCommits(&start, trackedIDs, relativePath, repository);
}

History::~History()
{
	SAFE_DELETE(historyGraph_);
}


void History::detectRelevantCommits(const CommitGraphItem* current, QSet<Model::NodeIdType> trackedIDs,
												QString relativePathRootNode, const GitRepository* repository)
{
	for (auto child : current->children_)
	{
		Diff diff = repository->diff(current->commitSHA1_, child->commitSHA1_);
		IdToChangeDescriptionHash changes = diff.changes();

		bool isRelevant = false;
		for (auto change : changes.values())
		{
			if (trackedIDs.contains(change->id()))
			{
				isRelevant = true;
				break;
			}
		}

		QString childsRelativePathRootNode = relativePathRootNode;
		IdToChangeDescriptionHash::iterator iter = changes.find(rootNodeId_);
		if (iter != changes.end())
		{
			ChangeDescription* change = iter.value();
			const GenericNode* rootNode = change->newNode();
			childsRelativePathRootNode = rootNode->persistentUnit()->name();
		}

		if (isRelevant)
		{
			relevantCommits_.insert(child->commitSHA1_);
			QSet<Model::NodeIdType> newTrackedIDs = trackSubtree(child->commitSHA1_, childsRelativePathRootNode,
																				  repository);

			detectRelevantCommits(child, newTrackedIDs, childsRelativePathRootNode, repository);
		}
		else
			detectRelevantCommits(child, trackedIDs, relativePathRootNode, repository);
	}
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
