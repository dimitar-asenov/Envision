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

#include "Merge.h"

#include "GitRepository.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "ModelBase/src/model/TreeManager.h"

namespace FilePersistence {

bool Merge::abort()
{
	if (stage_ != Stage::NoMerge && stage_ != Stage::Complete)
	{
		GitReference branch = repository_->currentBranch();

		repository_->checkout(head_, true);
		repository_->setReferenceTarget(branch, head_);

		stage_ = Stage::NoMerge;
		return true;
	}
	else
		return false;
}

bool Merge::commit(Signature author, Signature committer, QString message)
{
	if (stage_ == Stage::ReadyToCommit)
	{
		SHA1 treeSHA1 = repository_->writeIndexToTree();

		QStringList parents;
		parents.append(head_);
		parents.append(revision_);

		repository_->newCommit(treeSHA1, message, author, committer, parents);

		stage_ = Stage::Complete;

		return true;
	}
	else
		return false;
}

// ======== private ========

Merge::Merge(GitRepository* repository)
{
	repository_ = repository;
}

Merge::~Merge() {}

bool Merge::newMerge(RevisionString revision, bool fastForward)
{
	if (stage_ == Stage::NoMerge)
	{
		initialize(revision, fastForward);
		if (error_ != Error::NoError)
			return true;

		classifyKind();
		startMerging();

		return true;
	}
	else
		return false;
}

void Merge::initialize(RevisionString revision, bool fastForward)
{
	head_ = repository_->getSHA1("HEAD");
	revision_ = repository_->getSHA1(revision);

	fastForward_ = fastForward;

	mergeBase_ = repository_->findMergeBase(revision);
	if (mergeBase_.isNull())
		error_ = Error::NoMergeBase;

	stage_ = Stage::Initialized;
}

void Merge::classifyKind()
{
	if (mergeBase_.compare(revision_) == 0)
		kind_ = Merge::Kind::AlreadyUpToDate;
	else if (mergeBase_.compare(head_) == 0)
		kind_ = Merge::Kind::FastForward;
	else
		kind_ = Merge::Kind::TrueMerge;

	stage_ = Stage::Classified;
}

void Merge::startMerging()
{
	switch (kind_)
	{
		case Kind::AlreadyUpToDate:
			stage_ = Merge::Stage::Complete;
			break;

		case Merge::Kind::FastForward:
			if (fastForward_)
				performFastForward();
			else
			{
				repository_->writeRevisionIntoIndex(revision_);
				stage_ = Merge::Stage::ReadyToCommit;
			}
			break;

		case Merge::Kind::TrueMerge:
			// TODO
			Q_ASSERT(false);
			break;

		default:
			Q_ASSERT(false);
	}
}

void Merge::performFastForward()
{
	GitReference branch = repository_->currentBranch();

	repository_->setReferenceTarget(branch, revision_);
	stage_ = Merge::Stage::Complete;
}

void Merge::detectConflicts()
{
	baseHeadDiff_ = repository_->diff(mergeBase_, head_);
	baseRevisionDiff_ = repository_->diff(mergeBase_, revision_);

	loadMergeBase();
}

void Merge::loadMergeBase()
{
	repository_->checkout(mergeBase_, true);

	QString path = repository_->workdirPath();

	int index = path.lastIndexOf("/");
	index++;

	QString name = path.mid(index);

	QString pathWithoutName = path;
	pathWithoutName.remove(index, name.size());

	auto manager = new Model::TreeManager();
	manager->load(new FilePersistence::SimpleTextFileStore(pathWithoutName), name, false);
	manager->setName("Base version of merge");

	baseTree_ = manager;

	repository_->checkout(head_, true);
}

} /* namespace FilePersistence */
