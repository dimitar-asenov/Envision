/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
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
 **********************************************************************************************************************/

#include "CodeReviewManager.h"

#include "ModelBase/src/model/TreeManager.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"

namespace CodeReview {

const QString CodeReviewManager::CODE_REVIEW_COMMENTS_PREFIX = "CodeReviewComments_";

// TODO use versions to have a one code review manager for any combination of two versions
CodeReviewManager::CodeReviewManager(QString, QString)
{}

CodeReviewManager& CodeReviewManager::instance()
{
	// TODO add handling to create instances per version combination
	static CodeReviewManager manager{"", ""};
	return manager;
}

CommentedNode* CodeReviewManager::commentedNode(QString nodeId, QPoint offset)
{
	auto iter = commentedNodes_.constFind(nodeId);
	if (iter != commentedNodes_.constEnd()) return *iter;

	auto commentedNode = new CommentedNode{nodeId, offset};
	commentedNodes_.insert(nodeId, commentedNode);
	return commentedNode;
}

QList<QList<VersionControlUI::DiffFrame*>> CodeReviewManager::orderDiffFrames(
		GroupingFunction groupingFunction, OrderingFunction orderingFunction,
		QList<VersionControlUI::DiffFrame*> diffFrames)
{
	QList<QList<VersionControlUI::DiffFrame*>> result;
	auto groupedComparisonPairs = groupingFunction(diffFrames);
	for (auto group : groupedComparisonPairs)
	{
		auto orderedGroup = orderingFunction(group);
		result.prepend(orderedGroup);
	}
	return result;
}

void CodeReviewManager::saveReview(QString newVersion)
{
	auto store = new FilePersistence::SimpleTextFileStore{"."};
	auto list = new Model::List{};
	auto manager = new Model::TreeManager{CODE_REVIEW_COMMENTS_PREFIX+newVersion, list};

	manager->beginModification(list, "set");
	for (auto nodeID :  commentedNodes_.keys())
	{
		auto comment = commentedNodes_[nodeID];
		// if a parent was set, it was a list from an earlier save
		comment->setParent(nullptr);
		list->append(comment);
	}
	manager->endModification();
	manager->save(store);

}

QList<CommentedNode*> CodeReviewManager::loadReview(QString newVersion)
{
	// no comments to load
	if (!QDir{CODE_REVIEW_COMMENTS_PREFIX+newVersion}.exists()) return {};

	auto store = new FilePersistence::SimpleTextFileStore{"."};
	auto manager = new Model::TreeManager{};
	manager->load(store, CODE_REVIEW_COMMENTS_PREFIX+newVersion, false);
	auto list = manager->root();
	QList<CommentedNode*> result;
	for (auto listEntry : list->children())
	{
		auto comment = DCast<CommentedNode>(listEntry);
		commentedNodes_.insert(comment->nodeId()->get(), comment);
		result.append(comment);
	}

	return result;
}
}
