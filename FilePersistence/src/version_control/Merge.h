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

#include "Diff.h"
#include "Commit.h"
#include "ConflictPipelineComponent.h"
#include "ChangeDependencyGraph.h"

namespace FilePersistence {

using ChangeToChangeHash = QMultiHash<const std::shared_ptr<ChangeDescription>,
												  const std::shared_ptr<ChangeDescription>>;
using RelationAssignmentTrace = QList<RelationAssignmentTransition>;

class GitRepository;

class FILEPERSISTENCE_API Merge
{
	public:
		~Merge();

		enum class Kind {Unclassified, AlreadyUpToDate, FastForward, TrueMerge};
		Kind kind() const;

		enum class Stage {NoMerge, Initialized, Classified, ConflictsDetected, ReadyToCommit, Complete};
		Stage stage() const;

		enum class Error {NoError, NoMergeBase};
		Error error() const;

		bool abort();
		bool commit(const Signature& author, const Signature& committer, const QString& message);

		// deprecated, will be removed
		const std::unique_ptr<GenericTree> mergeTree();

	private:
		friend class GitRepository;

		Merge(QString revision, bool fastForward, GitRepository* repository);

		void classifyKind();

		void performTrueMerge();

		static QList<Model::NodeIdType> genericNodeListToNodeIdList(const QList<GenericNode*>& list);

		enum class ListType {NoList, OrderedList, UnorderedList};
		static ListType getListType(const GenericNode* node);
		static bool isListType(const GenericNode* node);

		Kind kind_{};
		Stage stage_{};
		Error error_{};

		// GenericTrees
		std::unique_ptr<GenericTree> treeBase_;
		std::unique_ptr<GenericTree> treeB_;
		std::unique_ptr<GenericTree> treeA_;

		bool fastForward_{};

		// Revisions
		QString headCommitId;
		QString revisionCommitId_;
		QString baseCommitId_;

		GitRepository* repository_{};

		/**
		 * This component is executed first, before any pipeline component.
		 * It establishes the pipeline invariant but may not depend on it holding beforehand.
		 */
		std::shared_ptr<ConflictPipelineComponent> pipelineInitializer_;
		/**
		 * Components are executed in the order they appear in this list.
		 */
		QList<std::shared_ptr<ConflictPipelineComponent>> conflictPipeline_;

		/**
		 * changes in this set cannot be applied safely.
		 */
		QSet<std::shared_ptr<const ChangeDescription>> conflictingChanges_;

		/**
		 * change1 is mapped to change2 iff change1 and change2 cannot both be applied safely.
		 */
		ConflictPairs conflictPairs_;
};

inline Merge::Kind Merge::kind() const { return kind_; }
inline Merge::Stage Merge::stage() const { return stage_; }
inline Merge::Error Merge::error() const { return error_; }

} /* namespace FilePersistence */
