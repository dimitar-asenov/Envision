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
#include "overlays/CodeReviewCommentOverlay.h"

#include "ModelBase/src/model/TreeManager.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/ViewItem.h"

namespace CodeReview {

const QString CodeReviewManager::CODE_REVIEW_COMMENTS_PREFIX = "CodeReviewComments_";

// TODO use versions to have a one code review manager for any combination of two versions
CodeReviewManager::CodeReviewManager(QString, QString)
{
	commentedNodes_ = new CommentedNodeList{};
}

CodeReviewManager& CodeReviewManager::instance()
{
	// TODO add handling to create instances per version combination
	static CodeReviewManager manager{"", ""};
	return manager;
}

CommentedNode* CodeReviewManager::commentedNode(QString nodeId, QString nodeManagerName, QPoint offset)
{
	auto commentedNode = commentedNodes_->find(nodeId);
	if (commentedNode) return commentedNode;

	commentedNode = new CommentedNode{nodeId, nodeManagerName, offset};
	commentedNodes_->beginModification();
	commentedNodes_->append(commentedNode);
	commentedNodes_->endModification();
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
	auto manager = new Model::TreeManager{CODE_REVIEW_COMMENTS_PREFIX+newVersion, commentedNodes_};
	manager->save(store);

}

CommentedNodeList* CodeReviewManager::loadReview(QString newVersion, VersionControlUI::DiffSetup& diffSetup,
																 Visualization::ViewItem* viewItem)
{
	// no comments to load
	if (!QDir{CODE_REVIEW_COMMENTS_PREFIX+newVersion}.exists()) return {};

	auto store = new FilePersistence::SimpleTextFileStore{"."};
	auto manager = new Model::TreeManager{};
	manager->load(store, CODE_REVIEW_COMMENTS_PREFIX+newVersion, false);
	commentedNodes_ = DCast<CommentedNodeList>(manager->root());
	Q_ASSERT(commentedNodes_);

	// recreate comment overlays
	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  [this, viewItem, diffSetup]()
	{
		for (auto comment : *commentedNodes_)
		{
			Model::Node* node = nullptr;
			auto managerName = comment->managerName()->get();
			if (managerName == diffSetup.newVersionManager_->managerName())
				node = const_cast<Model::Node*>(diffSetup.newVersionManager_->
															 nodeIdMap().node(comment->nodeId()->get()));
			else if (managerName == diffSetup.oldVersionManager_->managerName())
				node = const_cast<Model::Node*>(diffSetup.oldVersionManager_->
															 nodeIdMap().node(comment->nodeId()->get()));
			if (!node) continue;

			for (auto item : viewItem->findAllVisualizationsOf(node))
			{
				auto overlay = new CodeReviewCommentOverlay{item, comment};
				viewItem->addOverlay(overlay, "CodeReviewComment");
				registerCommentedNodeWithOverlay(comment, overlay);
			}
		}
	});

	return commentedNodes_;
}

Visualization::Item* CodeReviewManager::overlayForCommentedNode(Model::Node* commentedNode)
{
	return commentedNodeToOverlay_.value(commentedNode);
}

void CodeReviewManager::registerCommentedNodeWithOverlay(Model::Node* commentedNode, Visualization::Item* overlay)
{
	commentedNodeToOverlay_.insert(commentedNode, overlay);
}

}
