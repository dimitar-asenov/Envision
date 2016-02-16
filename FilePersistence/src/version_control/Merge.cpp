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
#include "../simple/SimpleTextFileStore.h"

namespace FilePersistence {

bool Merge::commit(const Signature& author, const Signature& committer, const QString& message)
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

std::shared_ptr<GenericTree> Merge::mergedTree()
{
	Q_ASSERT(stage_ >= Stage::BuiltMergedTree);
	return treeMerged_;
}

Merge::Merge(QString revision, bool fastForward, GitRepository* repository)
	: repository_{repository}
{
	// TODO: fill those lists correctly
	const QStringList listTypes{"StatementItemList",
										 "TypedListOfResult",
										 "TypedListOfFormalTypeArgument",
										 "TypedListOfExpression",
										 "TypedListOfDeclaration",
										 "TypedListOfMemberInitializer",
										 "TypedListOfEnumerator"
										};

	const QStringList unorderedTypes{"TypedListOfClass",
												"TypedListOfMethod",
												"TypedListOfFormalArgument",
												"TypedListOfFormalResult",
												"TypedListOfField",
												"TypedListOfUsedLibrary"
											  };

	const QStringList statements{"Block",
										  "BreakStatement",
										  "CaseStatement",
										  "ContinueStatement",
										  "DeclarationStatement",
										  "ExpressionStatement",
										  "ForEachStatement",
										  "IfStatement",
										  "LoopStatement",
										  "ReturnStatement",
										  "Statement",
										  "SwitchStatement",
										  "TryCatchFinallyStatement"};

	const QStringList declarations{"Class",
											 "Declaration",
											 "ExplicitTemplateInstantiation",
											 "Field",
											 "Method",
											 "Module",
											 "NameImport",
											 "Project",
											 "TypeAlias",
											 "VariableDeclaration"};

	const QStringList extraUnitTypes{"CommentStatementItem",
												"CatchClause",
												"StatementItemList"};

	const QStringList debugAndTestUnitTypes{"TestConflictType",
														 "TestListType",
														 "TestUnorderedType",
														 "ListElement",
														 "project",
														 "package",
														 "class",
														 "fieldList",
														 "methodList",
														 "field",
														 "method",
														 "Method",
														 "loop",
														 "TypedListOfMethod"};

	const QStringList debugAndTestListTypes{"TestListType",
														 "TestNoConflictList",
														 "project",
														 "package",
														 "fieldList",
														 "methodList",
														 "method"};

	const QStringList debugAndTestUnordered{"TestUnorderedType",
														 "TypedListOfMethod"};

	listTypes_ = QSet<QString>::fromList(listTypes + debugAndTestListTypes);
	unorderedTypes_ = QSet<QString>::fromList(unorderedTypes + debugAndTestUnordered);
	conflictTypes_ = listTypes_ + unorderedTypes_ +
			QSet<QString>::fromList(statements + declarations + extraUnitTypes + debugAndTestUnitTypes);


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
			stage_ = Merge::Stage::Committed;
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

Merge::~Merge()
{
	treeA_ = nullptr;
	treeB_ = nullptr;
	treeBase_ = nullptr;
	treeMerged_ = nullptr;
}

void Merge::initializeComponents()
{
	pipelineInitializer_ = std::shared_ptr<ConflictUnitDetector>(
				new ConflictUnitDetector{conflictTypes_, USE_LINKED_SETS});

	auto listMergeComponent = std::make_shared<ListMergeComponent>(conflictTypes_, listTypes_, unorderedTypes_);
	conflictPipeline_.append(listMergeComponent);
}

void Merge::performTrueMerge()
{
	initializeComponents();

	treeA_ = std::shared_ptr<GenericTree>(new GenericTree{repository_->projectName()});
	new GitPiecewiseLoader{treeA_, repository_, headCommitId_};
	treeB_ = std::unique_ptr<GenericTree>(new GenericTree{repository_->projectName()});
	new GitPiecewiseLoader{treeB_, repository_, revisionCommitId_};
	treeBase_ = std::unique_ptr<GenericTree>(new GenericTree{repository_->projectName()});
	new GitPiecewiseLoader{treeBase_, repository_, baseCommitId_};

	Diff diffA = repository_->diff(baseCommitId_, headCommitId_, treeBase_, treeA_);
	Diff diffB = repository_->diff(baseCommitId_, revisionCommitId_, treeBase_, treeB_);

	auto cdgA = ChangeDependencyGraph{diffA};
	auto cdgB = ChangeDependencyGraph{diffB};
	conflictingChanges_ = {};
	conflictPairs_ = {};

	LinkedChangesSet linkedChangesSet;
	if (USE_LINKED_SETS)
		linkedChangesSet = LinkedChangesSet{cdgA, cdgB};

	// ### PIPELINE INITIALIZER ###
	LinkedChangesTransition transition = pipelineInitializer_->run(treeA_, treeB_, treeBase_,
																						cdgA, cdgB, conflictingChanges_,
																						conflictPairs_, linkedChangesSet);
	// NOTE this is where one would check the transition of the initializer.

	// ### THE PIPLELINE ###
	for (auto component : conflictPipeline_)
	{
		// deep copy current state
		linkedChangesSet = LinkedChangesSet{transition.getNewState()};
		transition = component->run(treeA_, treeB_, treeBase_, cdgA, cdgB,
											 conflictingChanges_, conflictPairs_, linkedChangesSet);
		// NOTE this is where one would check the transition of a component.
	}

	IdToChangeDescriptionHash applicableChanges;
	for (auto change : cdgA.changes())
		if (!conflictingChanges_.contains(change) && !change->onlyStructureChange())
			applicableChanges.insert(change->nodeId(), change);
	for (auto change : cdgB.changes())
		if (!conflictingChanges_.contains(change) && !change->onlyStructureChange())
			applicableChanges.insert(change->nodeId(), change);

	stage_ = Stage::AutoMerged;

	treeMerged_ = std::shared_ptr<GenericTree>(new GenericTree{repository_->projectName()});
	repository_->loadGenericTree(treeMerged_, baseCommitId_);
	treeMerged_->buildLookupHash();
	applyChangesToTree(treeMerged_, cdgA);
	applyChangesToTree(treeMerged_, cdgB);

	// remove holes in lists
	for (auto changeIt = cdgA.changes().constBegin(); changeIt != cdgA.changes().constEnd(); ++changeIt)
	{
		const auto& change = changeIt.value();
		auto list = treeMerged_->find(change->nodeId());
		if (list && (listTypes_.contains(list->type()) ||
						 unorderedTypes_.contains(list->type())))
		{
			// list is a list container that has changed and exists in the merged tree
			int gapSize = 0;
			for (int curIdx = 0; curIdx < list->children().size(); ++curIdx)
			{
				GenericNode* elem;
				while (!(elem = list->child(QString::number(curIdx + gapSize))))
					++gapSize;
				elem->setLabel(QString::number(curIdx));
			}
		}
	}

	stage_ = Stage::BuiltMergedTree;

	SimpleTextFileStore::saveGenericTree(treeMerged_, repository_->projectName(),
													 repository_->workdirPath(), {"Project", "Module"});

	if (conflictingChanges_.isEmpty())
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

void Merge::addDependencies(QList<std::shared_ptr<ChangeDescription>>& queue,
									 const std::shared_ptr<ChangeDescription>& change,
									 const ChangeDependencyGraph& cdg)
{
	for (auto dependency : cdg.getDependencies(change))
		if (!queue.contains(dependency))
		{
			Q_ASSERT(!conflictingChanges_.contains(dependency));
			addDependencies(queue, dependency, cdg);
			queue.append(dependency);
		}
}


void Merge::applyChangesToTree(const std::shared_ptr<GenericTree>& tree,
										 const ChangeDependencyGraph& cdg)
{
	// sort changes topologically before applying
	QList<std::shared_ptr<ChangeDescription>> queue;
	for (auto change : cdg.changes())
		if (!conflictingChanges_.contains(change) && !queue.contains(change))
		{
			addDependencies(queue, change, cdg);
			queue.append(change);
		}

	// We apply changes in a way that make them indempotent.
	// This makes it possible for both branches to make the exact same change
	// e.g. delete the same node.

	for (auto change : queue)
		switch (change->type())
		{
			case ChangeType::Insertion:
			{
				auto existing = tree->find(change->nodeB()->id());
				if (existing)
					Q_ASSERT(existing->equalTo(change->nodeB()));
				else
				{
					auto persistentUnitName = change->nodeB()->persistentUnit()->name();
					auto persistentUnit = tree->persistentUnit(persistentUnitName);
					if (!persistentUnit)
						persistentUnit = &tree->newPersistentUnit(persistentUnitName);
					auto node = persistentUnit->newNode(change->nodeB());
					node->linkNode();
					Q_ASSERT(node->parent()->id() == change->nodeB()->parentId());
					Q_ASSERT(tree->find(change->nodeId()));
				}
				break;
			}

			case ChangeType::Deletion:
			{
				auto node = tree->find(change->nodeA()->id());
				if (node)
				{
					Q_ASSERT(node->children().empty());
					tree->remove(change->nodeId());
					Q_ASSERT(!tree->find(change->nodeA()->id()));
				}
				break;
			}

			case ChangeType::Move:
			{
				auto node = tree->find(change->nodeA()->id());
				node->detachFromParent();
				node->setParentId(change->nodeB()->parentId());
				node->attachToParent();
				Q_ASSERT(node->parent()->id() == change->nodeB()->parentId());
				// no break, need to do the same stuff as for stationary.
			}

			case ChangeType::Stationary:
			{
				auto node = tree->find(change->nodeA()->id());

				if (change->hasFlags(ChangeDescription::Value))
				{
					auto valueType = change->nodeB()->valueType();
					auto value = change->nodeB()->rawValue();
					node->resetValue(valueType, value);
				}

				if (change->hasFlags(ChangeDescription::Type))
					node->setType(change->nodeB()->type());

				if (change->hasFlags(ChangeDescription::Label))
					node->setLabel(change->nodeB()->label());

				break;
			}

			default:
				Q_ASSERT(false);
		}
}

}
