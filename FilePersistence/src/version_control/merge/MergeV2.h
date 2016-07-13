/***********************************************************************************************************************
**
** Copyright (c) 2016 ETH Zurich
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

#include "../../filepersistence_api.h"

#include "MergeData.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class GitRepository;
class MergePipelineComponent;
class GenericTree;
struct Signature;

class FILEPERSISTENCE_API MergeV2
{
	public:
		enum class Kind {Unclassified, AlreadyUpToDate, FastForward, TrueMerge};
		enum class Stage {NotInitialized, FoundMergeBase, Classified, AutoMerged,
								ManualMerged, BuiltMergedTree, WroteToWorkDir, WroteToIndex, Committed};

		bool isAlreadyMerged() const;
		bool hasConflicts() const;
		std::shared_ptr<GenericTree> mergedTree();
		bool commit(const Signature& author, const Signature& committer, const QString& message);

		// Used for testing
		bool isNodeInConflict(Model::NodeIdType nodeId) const;

	private:
		friend class GitRepository;

		/**
		 * Merges \a revision into current HEAD.
		 */
		MergeV2(QString revision, bool fastForward, GitRepository* repository);

		void initializePipelineComponents();

		/**
		 * If the merge is non-trivial, this is where the real merge algorithm and the pipeline is run.
		 */
		void performTrueMerge();

		Stage stage_ = Stage::NotInitialized;

		MergeData mergeData_;

		/**
		 * Revisions
		 */
		QString headCommitId_;
		QString revisionCommitId_;
		QString baseCommitId_;

		GitRepository* repository_{};

		/**
		 * Components are executed in the order they appear in this list.
		 */
		QList<std::shared_ptr<MergePipelineComponent>> mergePipeline_;

};

inline bool MergeV2::hasConflicts() const { return mergeData_.cg_.hasConflicts(); }
inline bool MergeV2::isAlreadyMerged() const { return stage_ == Stage::Committed; }

}
