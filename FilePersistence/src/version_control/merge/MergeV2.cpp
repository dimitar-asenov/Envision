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

#include "MergeV2.h"

#include "MergePipelineComponent.h"
#include "ConflictUnitComponent.h"
#include "ListMergeComponentV2.h"

#include "../Diff.h"
#include "../GitPiecewiseLoader.h"
#include "../GitRepository.h"
#include "../../simple/GenericNode.h"
#include "../../simple/GenericTree.h"
#include "../../simple/SimpleTextFileStore.h"

namespace FilePersistence {

bool MergeV2::commit(const Signature& author, const Signature& committer, const QString& message)
{
	Q_ASSERT(stage_ == Stage::WroteToIndex);

	QString treeSHA1 = repository_->writeIndexToTree();

	QStringList parents;
	parents.append(headCommitId_);
	parents.append(revisionCommitId_);

	repository_->newCommit(treeSHA1, message, author, committer, parents);

	stage_ = Stage::Committed;

	return true;
}

std::shared_ptr<GenericTree> MergeV2::mergedTree()
{
	Q_ASSERT(stage_ >= Stage::BuiltMergedTree);
	return mergeData_.treeMerged_;
}

MergeV2::MergeV2(QString revision, bool fastForward, GitRepository* repository)
	: repository_{repository}
{
	headCommitId_ = repository_->getSHA1("HEAD");
	revisionCommitId_ = repository_->getSHA1(revision);
	baseCommitId_ = repository_->findMergeBase("HEAD", revision);

	Q_ASSERT(!baseCommitId_.isNull());

	stage_ = Stage::FoundMergeBase;

	Kind kind;
	if (baseCommitId_.compare(revisionCommitId_) == 0) kind = Kind::AlreadyUpToDate;
	else if (baseCommitId_.compare(headCommitId_) == 0) kind = Kind::FastForward;
	else kind = Kind::TrueMerge;

	stage_ = Stage::Classified;

	switch (kind)
	{
		case Kind::AlreadyUpToDate:
		{
			stage_ = MergeV2::Stage::Committed;
			break;
		}
		case Kind::FastForward:
		{
			if (fastForward)
			{
				QString branch = repository_->currentBranch();
				repository_->setReferenceTarget(branch, revisionCommitId_);
				repository_->checkout(revisionCommitId_, true);
				stage_ = Stage::Committed;
			}
			else
			{
				repository_->checkout(revisionCommitId_, true);
				repository_->writeRevisionIntoIndex(revisionCommitId_);
				stage_ = Stage::WroteToIndex;
			}
			break;
		}
		case Kind::TrueMerge:
		{
			performTrueMerge();
			break;
		}
		default:
			Q_ASSERT(false);
	}
}

void MergeV2::initializePipelineComponents()
{
	mergePipeline_.append(std::make_shared<ConflictUnitComponent>());
	mergePipeline_.append(std::make_shared<ListMergeComponentV2>());
}

void MergeV2::performTrueMerge()
{
	mergeData_.treeA_ = std::shared_ptr<GenericTree>(new GenericTree{repository_->projectName()});
	new GitPiecewiseLoader{mergeData_.treeA_, repository_, headCommitId_};
	mergeData_.treeB_ = std::unique_ptr<GenericTree>(new GenericTree{repository_->projectName()});
	new GitPiecewiseLoader{mergeData_.treeB_, repository_, revisionCommitId_};
	mergeData_.treeBase_ = std::unique_ptr<GenericTree>(new GenericTree{repository_->projectName()});
	new GitPiecewiseLoader{mergeData_.treeBase_, repository_, baseCommitId_};

	mergeData_.treeMerged_ = std::shared_ptr<GenericTree>(new GenericTree{repository_->projectName()});
	// We will fully load the merge tree. Therefore there is no need to provide a piecewise loader.
	repository_->loadGenericTree(mergeData_.treeMerged_, baseCommitId_);
	mergeData_.treeMerged_->buildLookupHash();

	Diff diffA = repository_->diff(baseCommitId_, headCommitId_, mergeData_.treeBase_, mergeData_.treeA_);
	Diff diffB = repository_->diff(baseCommitId_, revisionCommitId_, mergeData_.treeBase_, mergeData_.treeB_);

	// We need to use the mergedTree below, since it has to be fully loaded (otherwise tree->find(x) won't work).
	mergeData_.cg_.init(diffA, diffB, mergeData_.treeMerged_.get());

	// Run the pipeline
	initializePipelineComponents();
	for (auto component : mergePipeline_)
		component->run(mergeData_);

	stage_ = Stage::AutoMerged;

	mergeData_.applyNonConflictingChanges();

	stage_ = Stage::BuiltMergedTree;

	SimpleTextFileStore::saveGenericTree(mergeData_.treeMerged_, repository_->projectName(),
													 repository_->workdirPath(), {"Project", "Module"});

	if (!hasConflicts())
	{
		stage_ = Stage::WroteToWorkDir;

		repository_->writeWorkdirToIndex();

		stage_ = Stage::WroteToIndex;
	}
	else
	{
		// TODO prepare for manual merge
		// TODO write conflicts to file maybe.
	}
}

bool MergeV2::isNodeInConflict(Model::NodeIdType nodeId) const
{
	//TODO : add soft conflicts
	return !mergeData_.cg_.changesForNode(nodeId).isEmpty();
}

bool MergeV2::hasConflicts() const
{
	// TODO: consider soft conflicts
	return !mergeData_.cg_.changes().isEmpty();
}

}
