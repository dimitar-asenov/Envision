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
#include "commands/CFocus.h"

#include "ModelBase/src/model/TreeManager.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/ViewItem.h"

namespace CodeReview {

const QString CodeReviewManager::CODE_REVIEW_COMMENTS_PREFIX = "CodeReviewComments_";

// TODO use versions to have a one code review manager for any combination of two versions
CodeReviewManager::CodeReviewManager(QString, QString)
{
	nodeReviews_ = new NodeReviewsList{};
}

CodeReviewManager& CodeReviewManager::instance()
{
	// TODO add handling to create instances per version combination
	static CodeReviewManager manager{"", ""};
	return manager;
}

NodeReviews* CodeReviewManager::nodeReviews(QString nodeId, QString revisionName, QPoint offset)
{
	auto nodeReviews = nodeReviews_->find(nodeId);
	if (nodeReviews) return nodeReviews;

	nodeReviews = new NodeReviews{nodeId, revisionName, offset};
	nodeReviews_->beginModification();
	nodeReviews_->append(nodeReviews);
	nodeReviews_->endModification();
	return nodeReviews;
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

void CodeReviewManager::saveReview(QString managerName, QString newRev)
{
	auto store = new FilePersistence::SimpleTextFileStore{"."};
	auto name = createNameForPersistence(managerName, newRev);
	auto manager = new Model::TreeManager{name, nodeReviews_};
	manager->save(store);
}

QString CodeReviewManager::createNameForPersistence(QString managerName, QString newRev)
{
	return CODE_REVIEW_COMMENTS_PREFIX + managerName + "_" + newRev;
}

QString CodeReviewManager::createNameForPersistence(const VersionControlUI::DiffSetup& diffSetup)
{
	return createNameForPersistence(diffSetup.managerName_, diffSetup.newVersion_);
}

NodeReviewsList* CodeReviewManager::loadReview(const VersionControlUI::DiffSetup& diffSetup,
																 Visualization::ViewItem* viewItem)
{
	CFocus::clearFocusInformation();

	// no comments to load
	if (!QDir{createNameForPersistence(diffSetup)}.exists()) return {};

	auto store = new FilePersistence::SimpleTextFileStore{"."};
	auto manager = new Model::TreeManager{};
	auto name = createNameForPersistence(diffSetup);
	manager->load(store, name, false);
	nodeReviews_ = DCast<NodeReviewsList>(manager->root());
	Q_ASSERT(nodeReviews_);

	// recreate comment overlays
	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  [this, viewItem, diffSetup]()
	{
		for (auto comment : *nodeReviews_)
		{
			Model::Node* node = nullptr;
			auto revisionName = comment->revisionName();
			if (revisionName == diffSetup.newVersionManager_->revisionName())
				node = const_cast<Model::Node*>(diffSetup.newVersionManager_->
															 nodeIdMap().node(comment->nodeId()));
			else if (revisionName == diffSetup.oldVersionManager_->revisionName())
				node = const_cast<Model::Node*>(diffSetup.oldVersionManager_->
															 nodeIdMap().node(comment->nodeId()));
			if (!node) continue;

			for (auto item : viewItem->findAllVisualizationsOf(node))
			{
				auto overlay = new CodeReviewCommentOverlay{item, comment};
				viewItem->addOverlay(overlay, "CodeReviewComment");
				registerNodeReviewsWithOverlay(comment, overlay);
			}
		}
	});

	return nodeReviews_;
}

Visualization::Item* CodeReviewManager::overlayForNodeReviews(Model::Node* nodeReviews)
{
	return nodeReviewsToOverlay_.value(nodeReviews);
}

void CodeReviewManager::registerNodeReviewsWithOverlay(Model::Node* nodeReviews, Visualization::Item* overlay)
{
	nodeReviewsToOverlay_.insert(nodeReviews, overlay);
}


}
