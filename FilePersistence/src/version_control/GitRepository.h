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

struct git_repository;
struct git_tree;
struct git_commit;
struct git_oid;

namespace FilePersistence {

class FILEPERSISTENCE_API GitRepository
{
	public:
		GitRepository(QString path);
		~GitRepository();

		Diff diff(QString oldCommit, QString newCommit) const;
		CommitGraph commitGraph(QString start, QString end) const;

		Commit getCommit(QString commitSpec) const;
		CommitFile getCommitFile(QString commitSpec, QString relativePath) const;
		CommitMetaData getCommitInformation(QString commitSpec) const;

		QString getSHA(QString commit) const;

		void checkout(QString commit, bool force);

		static const QString WORKDIR;
		static const QString INDEX;

	private:
		void traverseCommitGraph(CommitGraph* graph, git_commit* current, const git_oid* target) const;

		git_commit* parseCommit(QString commit) const;

		enum class DiffKind {Unspecified, IndexToWorkdir, CommitToWorkdir, CommitToIndex, CommitToCommit};
		static DiffKind kind(QString oldCommit, QString newCommit);

		static void checkError(int errorCode);

		QString path_;
		git_repository* repository_{};

};

} /* namespace FilePersistence */
