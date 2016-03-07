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

#pragma once
#include "../filepersistence_api.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class GitRepository;
class Diff;
class CommitGraph;
class GenericTree;
struct CommitGraphItem;

class FILEPERSISTENCE_API History
{
	public:
		History(QString relativePath, Model::NodeIdType rootNodeId,
				  const CommitGraph* historyGraph, const GitRepository* repository);

		QList<QString> relevantCommitsByTime(const GitRepository* repository, bool reverse = true) const;

		QSet<QString> relevantCommits() const;

	private:
		void detectRelevantCommits(const CommitGraphItem* current, QSet<const CommitGraphItem*> visited,
											QString relativePathRootNode, QSet<Model::NodeIdType> trackedIDs,
											const GitRepository* repository);

		QSet<Model::NodeIdType> trackSubtree(QString revision, QString relativePath, GenericTree* tree,
														 const GitRepository* repository) const;

		QString findRootPath(QString revision, QString currentPath, const Diff* diff, GenericTree* tree,
									const GitRepository* repository);

		struct CommitTime
		{
				QString commitSHA1_;
				QDateTime dateTime_;

				CommitTime(QString sha1, QDateTime dateTime);

				bool operator < (const CommitTime& commitTime) const;
				bool operator > (const CommitTime& commitTime) const;

				static bool later(const CommitTime& left, const CommitTime& right);
		};

		Model::NodeIdType rootNodeId_;
		const CommitGraph* historyGraph_{};
		QSet<QString> relevantCommits_;
};

inline QSet<QString> History::relevantCommits() const { return relevantCommits_; }
}
