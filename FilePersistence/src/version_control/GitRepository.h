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

#pragma once

#include "Commit.h"
#include "Diff.h"
#include "CommitGraph.h"
#include "Merge.h"

struct git_repository;
struct git_tree;
struct git_commit;
struct git_oid;

namespace FilePersistence {

using GitReference = QString;
using SHA1 = QString;
using RevisionString = QString;

class FILEPERSISTENCE_API GitRepository
{
	public:
		GitRepository(QString path);
		~GitRepository();

		bool isMerging() const;
		Merge* merge(QString revision, bool useFastForward = true);
		bool abortMerge();
		bool commitMerge(Signature committer, QString commitMessage);

		Diff diff(QString oldRevision, QString newRevision) const;
		CommitGraph commitGraph(QString startRevision, QString endRevision) const;

		const Commit* getCommit(QString revision) const;
		const CommitFile* getCommitFile(QString revision, QString relativePath) const;
		CommitMetaData getCommitInformation(QString revision) const;

		void checkout(QString revision, bool force);

		QString workdirPath() const;

		SHA1 getSHA1(QString revision) const;

		enum class HEADState {UNBORN, ATTACHED, DETACHED};
		HEADState getHEADState() const;

		enum class GitReferenceType {INVALID, NOTFOUND, BRANCH, NOTE, REMOTE, TAG};
		GitReferenceType referenceType(GitReference reference) const;

		GitReference currentBranch() const;

		QStringList localBranches() const;
		QStringList tags() const;
		QStringList revisions() const;

		enum class RevisionStringState {INVALID, NOTFOUND, AMBIGUOUS, VALID};
		bool isValidRevisionString(RevisionString revision) const;

		static const QString WORKDIR;
		static const QString INDEX;

	private:
		void traverseCommitGraph(CommitGraph* graph, git_commit* current, const git_oid* target) const;

		const CommitFile* getCommitFileFromWorkdir(QString relativePath) const;
		const CommitFile* getCommitFileFromIndex(QString relativePath) const;
		const CommitFile* getCommitFileFromTree(QString revision, QString relativePath) const;

		git_commit* parseCommit(QString revision) const;

		const git_oid* buildTreeFromWorkdir();

		bool hasCleanIndex() const;
		bool hasCleanWorkdir() const;

		static Merge::Kind classifyMerge(const git_oid* revision, const git_oid* head, const git_oid* mergeBase);

		static const QString REFS_HEADS_PATH;
		void setBranchHeadToCommit(QString branch, QString revision);

		enum class DiffKind {Unspecified, WorkdirToWorkdir, WorkdirToIndex, WorkdirToCommit,
									IndexToWorkdir, IndexToIndex, IndexToCommit,
									CommitToWorkdir, CommitToIndex, CommitToCommit};
		static DiffKind kind(QString oldRevision, QString newRevision);

		static void checkError(int errorCode);

		static const char* HEAD;

		Merge* currentMerge_{};

		QString path_;
		git_repository* repository_{};

};

inline bool GitRepository::isMerging() const { return (currentMerge_ != nullptr); }
inline QString GitRepository::workdirPath() const { return path_; }

} /* namespace FilePersistence */
