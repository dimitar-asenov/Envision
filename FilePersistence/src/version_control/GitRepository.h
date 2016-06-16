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

#include "Commit.h"
#include "CommitGraph.h"
#include "Diff.h"
#include "Merge.h"

struct git_repository;
struct git_tree;
struct git_commit;
struct git_oid;
struct git_signature;

namespace FilePersistence {

class GenericNode;
class GenericTree;

class FILEPERSISTENCE_API GitRepository
{
	public:
		GitRepository(QString path);
		~GitRepository();

		static bool repositoryExists(QString path);

		std::shared_ptr<Merge> merge(QString revision, bool fastForward = true);

		Diff diff(QString revisionA, QString revisionB,
					 std::shared_ptr<GenericTree> treeA = nullptr,
					 std::shared_ptr<GenericTree> treeB = nullptr) const;
		CommitGraph commitGraph(QString startRevision, QString endRevision) const;

		const Commit* getCommit(QString revision) const;
		const CommitFile* getCommitFile(QString revision, QString relativePath) const;
		CommitMetaData getCommitInformation(QString revision) const;

		void checkout(QString revision, bool force);

		QString workdirPath() const;

		QString  getSHA1(QString revision) const;

		enum class HEADState {UNBORN, ATTACHED, DETACHED};
		HEADState getHEADState() const;

		enum class GitReferenceType {INVALID, NOTFOUND, BRANCH, NOTE, REMOTE, TAG};
		GitReferenceType referenceType(QString reference) const;

		QString currentBranchName() const;

		QStringList localBranches() const;
		QStringList tags() const;
		QStringList revisions() const;

		enum class RevisionStringState {INVALID, NOTFOUND, AMBIGUOUS, VALID};
		bool isValidRevisionString(QString  revision) const;

		static const QString WORKDIR;
		static const QString INDEX;

		void loadGenericTree(const std::shared_ptr<GenericTree>& tree, const QString version);

		static int getMinPrefixLength();

		QString relativePathForPersistentUnit(QString persistentUnitId, QString revision) const;

	private:
		friend class Merge;

		QString projectName() const;

		void writeRevisionIntoIndex(QString  revision);
		QString  writeIndexToTree();
		void writeWorkdirToIndex();

		void newCommit(QString  tree, QString message, Signature author, Signature committer, QStringList parents);
		QString  findMergeBase(QString  branchA, QString branchB) const;

		git_signature* createGitSignature(Signature& signature);

		static const QString PATH_HEADS;
		static const QString PATH_REMOTES;
		static const QString PATH_TAGS;
		static const QString PATH_NOTES;

		QString currentBranch() const;
		bool setReferenceTarget(QString reference, QString  target);

		void TraverseCommitGraph(CommitGraph* graph, git_commit* current, const git_oid* target) const;

		const CommitFile* getCommitFileFromWorkdir(QString relativePath) const;
		const CommitFile* getCommitFileFromIndex(QString relativePath) const;
		const CommitFile* getCommitFileFromTree(QString revision, QString relativePath) const;

		git_commit* parseCommit(QString revision) const;

		QString oidToQString(const git_oid* oid) const;

		void findPersistentUnitDeclarations(GenericNode* node, IdToGenericNodeHash& declarations);

		bool hasCleanIndex() const;
		bool hasCleanWorkdir() const;

		enum class SourceKind {Unspecified, Commit, Index, Workdir};
		static SourceKind sourceKind(QString revision);
		static QPair<SourceKind, SourceKind> kind(QString revisionA, QString revisionB);

		/**
		 * This is a wrapper around \a git_commit_tree().
		 *
		 * Returns the \a git_tree at \a revision.
		 */
		git_tree* getCommitTree(QString revision) const;

		/**
		 * \a errorCode is the last error code returned from libgit2.
		 *
		 * Throws an exception with a descriptive error description if \a errorCode indicates that an error occurred.
		 */
		static void checkError(int errorCode);

		static const char* HEAD;

		std::weak_ptr<Merge> merge_;

		QString path_;
		git_repository* repository_{};
};

inline QString GitRepository::workdirPath() const { return path_; }
inline QString GitRepository::projectName() const { return QFileInfo{path_}.fileName(); }

struct FILEPERSISTENCE_API GitDiffExtract
{
	GenericTree* treeA_{};
	GenericTree* treeB_{};

	QList<GenericNode*> nodesA_;
	QList<GenericNode*> nodesB_;

	bool reverseAB_{};
};

struct FILEPERSISTENCE_API GitCommitExtract
{
		git_repository* repository_{};
		Commit* commit_{};
};

struct FILEPERSISTENCE_API RelativePathData
{
		git_repository* repository_{};
		QString id;
		QString relativePath;
};


}
