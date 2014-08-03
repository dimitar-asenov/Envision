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

#include <QString>

#include "Diff.h"

namespace FilePersistence {

class GitRepository;

class Merge
{
	public:
		enum class Kind {Unclassified, AlreadyUpToDate, FastForward, TrueMerge};
		enum class Stage {Initial, Classified, ConflictsDetected, ReadyToCommit, Complete};
		enum class Error {NoError, NoMergeBase};

	private:
		friend class GitRepository;

		Merge(QString revision, bool useFastForward, GitRepository* repository);
		~Merge();

		void detectConflicts();

		void loadMergeBase();

		Kind kind_{};
		Stage stage_{};
		Error error_{};

		bool useFastForward_{};

		Model::TreeManager* baseTree_;

		// Diffs to base version
		Diff baseRevisionDiff_;
		Diff baseHeadDiff_;

		// Revisions
		QString head_;
		QString revision_;
		QString mergeBase_;

		GitRepository* repository_{};
};

} /* namespace FilePersistence */
