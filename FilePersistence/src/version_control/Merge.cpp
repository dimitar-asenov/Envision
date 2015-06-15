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

#include "Merge.h"
#include "GitRepository.h"
#include "GitPiecewiseLoader.h"

#include "ConflictUnitDetector.h"
#include "ListMergeComponent.h"

namespace FilePersistence {

bool Merge::commit(const Signature& author, const Signature& committer, const QString& message)
{
	if (stage_ == Stage::WroteToIndex)
	{
		QString treeSHA1 = repository_->writeIndexToTree();

		QStringList parents;
		parents.append(headCommitId_);
		parents.append(revisionCommitId_);

		repository_->newCommit(treeSHA1, message, author, committer, parents);

		stage_ = Stage::Committed;

		return true;
	}
	else
		return false;
}

// ======== private ========

Merge::Merge(QString revision, bool fastForward, GitRepository* repository)
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
			stage_ = Merge::Stage::Committed;
			break;

		case Kind::FastForward:
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

		case Kind::TrueMerge:
			performTrueMerge();
			break;

		default:
			Q_ASSERT(false);
	}
}

void Merge::initializeComponents()
{
	// TODO replace with correct types
	QSet<QString> conflictTypes = QSet<QString>::fromList(QList<QString>{
																				"TestConflictType",
																				"TestListType",
																				"TestUnorderedType",
																				"ListElement"
																			});
	QSet<QString> listTypes = QSet<QString>::fromList(QList<QString>{"TestListType", "TestNoConflictList"});
	QSet<QString> unorderedTypes = QSet<QString>::fromList(QList<QString>{"TestUnorderedType"});

	pipelineInitializer_ = std::make_shared<ConflictUnitDetector>(conflictTypes, headCommitId_,
																					  revisionCommitId_, baseCommitId_);

	auto listMergeComponent = std::make_shared<ListMergeComponent>(conflictTypes, listTypes, unorderedTypes);
	conflictPipeline_.append(listMergeComponent);
}

void Merge::performTrueMerge()
{
	initializeComponents();

	treeA_ = std::shared_ptr<GenericTree>(new GenericTree("TreeA"));
	new GitPiecewiseLoader(treeA_, repository_, headCommitId_);
	treeB_ = std::unique_ptr<GenericTree>(new GenericTree("TreeB"));
	new GitPiecewiseLoader(treeB_, repository_, revisionCommitId_);
	treeBase_ = std::unique_ptr<GenericTree>(new GenericTree("TreeBase"));
	new GitPiecewiseLoader(treeBase_, repository_, baseCommitId_);

	Diff diffA = repository_->diff(baseCommitId_, headCommitId_, treeBase_, treeA_);
	Diff diffB = repository_->diff(baseCommitId_, revisionCommitId_, treeBase_, treeB_);

	auto cdgA = ChangeDependencyGraph(diffA);
	auto cdgB = ChangeDependencyGraph(diffB);
	conflictingChanges_ = {};
	conflictPairs_ = {};

	LinkedChangesSet linkedChangesSet(cdgA, cdgB);

	// ### PIPELINE INITIALIZER ###
	LinkedChangesTransition transition = pipelineInitializer_->run(treeA_, treeB_, treeBase_,
																						cdgA, cdgB, conflictingChanges_,
																						conflictPairs_, linkedChangesSet);
	// NOTE this is where one would check the transition of the initializer.

	// ### THE PIPLELINE ###
	for (auto component : conflictPipeline_)
	{
		linkedChangesSet = LinkedChangesSet(transition.getNewState()); // deep copy current state
		transition = component->run(treeA_, treeB_, treeBase_, cdgA, cdgB,
											 conflictingChanges_, conflictPairs_, linkedChangesSet);
		// NOTE this is where one would check the transition of a component.
	}

	IdToChangeDescriptionHash applicableChanges;
	for (auto change : cdgA.changes())
		if (!conflictingChanges_.contains(change)) applicableChanges.insert(change->nodeId(), change);
	for (auto change : cdgB.changes())
		if (!conflictingChanges_.contains(change))
		{
			Q_ASSERT(!applicableChanges.contains(change->nodeId())); // no change for that ID exists
			applicableChanges.insert(change->nodeId(), change);
		}

	stage_ = Stage::AutoMerged;

	if (conflictingChanges_.isEmpty())
	{
		treeMerged_ = std::shared_ptr<GenericTree>(new GenericTree("Merged"));
		repository_->loadGenericTree(treeMerged_, baseCommitId_);
		applyChangesToTree(treeMerged_, cdgA);
		applyChangesToTree(treeMerged_, cdgB);

		stage_ = Stage::BuiltMergedTree;

		// TODO encode and write tree to working directory
		stage_ = Stage::WroteToWorkDir;
		// await commit
	}
	else
	{
		// TODO prepare for manual merge
		stage_ = Stage::ManualMerged;
	}
}

void Merge::addDependencies(QList<std::shared_ptr<ChangeDescription>>& queue,
									 const std::shared_ptr<ChangeDescription>& change,
									 const ChangeDependencyGraph& cdg)
{
	for (auto dependency : cdg.getDependencies(change))
	{
		if (!queue.contains(dependency))
		{
			Q_ASSERT(!conflictingChanges_.contains(dependency));
			addDependencies(queue, dependency, cdg);
			queue.append(dependency);
		}
	}
}


void Merge::applyChangesToTree(const std::shared_ptr<GenericTree>& tree,
										 const ChangeDependencyGraph& cdg)
{
	// sort changes topologically before applying
	QList<std::shared_ptr<ChangeDescription>> queue;
	for (auto change : cdg.changes())
	{
		if (!conflictingChanges_.contains(change) && !queue.contains(change))
		{
			addDependencies(queue, change, cdg);
			queue.append(change);
		}
	}


	for (auto change : queue)
	{
		switch (change->type())
		{
			case ChangeType::Insertion:
			{
				auto persistentUnitName = change->nodeB()->persistentUnit()->name();
				auto persistentUnit = tree->persistentUnit(persistentUnitName);
				if (!persistentUnit)
					persistentUnit = &tree->newPersistentUnit(persistentUnitName);
				auto node = persistentUnit->newNode(change->nodeB());
				node->linkNode();
				Q_ASSERT(node->parent()->id() == change->nodeB()->parentId());
				break;
			}

			case ChangeType::Deletion:
			{
				auto node = tree->find(change->nodeA()->id());
				Q_ASSERT(node->children().empty());
				node->remove();
				break;
			}

			case ChangeType::Move:
			{
				// TODO currently assuming no change of persistent unit
				auto node = tree->find(change->nodeA()->id());
				node->detachFromParent();
				node->reset(change->nodeB());
				node->linkNode(); // TODO only attach new parent
				break;
			}

			case ChangeType::Stationary:
			{
				// TODO currently assuming no change of persistent unit
				auto node = tree->find(change->nodeA()->id());
				node->reset(change->nodeB()); // FIXME should not unlink
				break;
			}

			default:
				Q_ASSERT(false);
		}
	}
}

} /* namespace FilePersistence */
