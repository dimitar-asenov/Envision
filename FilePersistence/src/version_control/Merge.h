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
#include "ConflictPairs.h"

namespace FilePersistence {

using ChangeToChangeHash = QMultiHash<const std::shared_ptr<ChangeDescription>,
												  const std::shared_ptr<ChangeDescription>>;
using LinkedChangesTransitionTrace = QList<LinkedChangesTransition>;

class GitRepository;

class FILEPERSISTENCE_API Merge
{
	public:
		~Merge();

		enum class Kind {Unclassified, AlreadyUpToDate, FastForward, TrueMerge};
		enum class Stage {NotInitialized, FoundMergeBase, Classified, AutoMerged,
								ManualMerged, BuiltMergedTree, WroteToWorkDir, WroteToIndex, Committed};

		bool hasConflicts() const;
		const QSet<std::shared_ptr<ChangeDescription>> getConflicts() const;
		std::shared_ptr<GenericTree> mergedTree();
		bool commit(const Signature& author, const Signature& committer, const QString& message);

		/**
		 * TODO output conflicts
		 */
		const bool USE_LINKED_SETS = true;

	private:
		friend class GitRepository;

		/**
		 * Merges \a revision into current HEAD.
		 */
		Merge(QString revision, bool fastForward, GitRepository* repository);

		/**
		 * Constructs components and sets special node types. This will eventually be replaced by a mechanism
		 * that loads these from plugins and the model.
		 */
		void initializeComponents();

		/**
		 * If the merge is non-trivial, this is where the magic starts.
		 */
		void performTrueMerge();

		void applyChangesToTree(const std::shared_ptr<GenericTree>& tree,
										const ChangeDependencyGraph& cdg);

		/**
		 * Computes transitive closure of dependencies for \a change.
		 */
		void addDependencies(QList<std::shared_ptr<ChangeDescription>>& queue,
									const std::shared_ptr<ChangeDescription>& change,
									const ChangeDependencyGraph& cdg);

		Stage stage_ = Stage::NotInitialized;

		/**
		 * GenericTrees
		 */
		std::shared_ptr<GenericTree> treeA_;
		std::shared_ptr<GenericTree> treeB_;
		std::shared_ptr<GenericTree> treeBase_;
		std::shared_ptr<GenericTree> treeMerged_;

		/**
		 * Revisions
		 */
		QString headCommitId_;
		QString revisionCommitId_;
		QString baseCommitId_;

		GitRepository* repository_{};

		/**
		 * This component is executed first, before any pipeline component.
		 * It establishes the pipeline invariant but must not depend on it holding beforehand.
		 */
		std::shared_ptr<ConflictPipelineComponent> pipelineInitializer_;

		/**
		 * Components are executed in the order they appear in this list.
		 */
		QList<std::shared_ptr<ConflictPipelineComponent>> conflictPipeline_;

		/**
		 * Changes in this set cannot be applied safely.
		 * Every change in the set should either be matched with a conflicting change in \a conflictingChanges_
		 * or be depending on a change that is in conflict. This is not enforced, however.
		 */
		QSet<std::shared_ptr<ChangeDescription>> conflictingChanges_;

		/**
		 * \a change1 is mapped to \a change2 exactly if \a change1 and \a change2 cannot both be applied safely.
		 */
		ConflictPairs conflictPairs_;


		QSet<QString> conflictTypes_;
		QSet<QString> listTypes_;
		QSet<QString> unorderedTypes_;
};

inline bool Merge::hasConflicts() const { return !conflictingChanges_.isEmpty(); }

inline const QSet<std::shared_ptr<ChangeDescription>> Merge::getConflicts() const { return conflictingChanges_; }

}
