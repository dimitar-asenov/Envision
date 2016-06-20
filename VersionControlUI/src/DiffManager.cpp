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

#include "DiffManager.h"
#include "VersionControlUIException.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/nodes/ViewItemNode.h"
#include "VisualizationBase/src/ViewItemManager.h"
#include "VisualizationBase/src/declarative/GridLayouter.h"
#include "VisualizationBase/src/overlays/HighlightOverlay.h"
#include "VisualizationBase/src/overlays/ArrowOverlay.h"
#include "VisualizationBase/src/overlays/IconOverlay.h"
#include "VisualizationBase/src/overlays/MessageOverlay.h"
#include "VisualizationBase/src/views/MainView.h"

#include "FilePersistence/src/version_control/GitRepository.h"
#include "FilePersistence/src/simple/SimpleTextFileStore.h"

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/expressions/EmptyExpression.h"

#include "ModelBase/src/model/AllTreeManagers.h"
#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Reference.h"

#include "nodes/DiffComparisonPair.h"
#include "items/VDiffComparisonPair.h"

namespace VersionControlUI
{

struct ChangeWithNodes {
	Model::NodeIdType id_;
	Model::Node* oldNode_{};
	Model::Node* newNode_{};
	FilePersistence::ChangeType changeType_{FilePersistence::ChangeType::Unclassified};
};

struct DiffSetup {
	Model::TreeManager* newVersionManager_{};
	Model::TreeManager* oldVersionManager_{};
	FilePersistence::GitRepository* repository_{};

	QString oldVersion_;
	QString newVersion_;
};

DiffManager::DiffManager(QString project, QList<Model::SymbolMatcher> contextUnitMatcherPriorityList,
								 Model::NodeIdType targetNodeID) : project_{project},
	contextUnitMatcherPriorityList_{contextUnitMatcherPriorityList}, targetNodeID_{targetNodeID}
{}

DiffSetup DiffManager::initializeDiffPrerequisites(QString oldVersion, QString newVersion)
{
	QString projectsDir = "projects/" + project_;

	// get GitRepository
	if (!FilePersistence::GitRepository::repositoryExists(projectsDir))
		throw VersionControlUIException{"Diff setup not possible. No repository found at " + projectsDir};

	DiffSetup diffSetup{};

	diffSetup.oldVersion_ = oldVersion;
	diffSetup.newVersion_ = newVersion;

	diffSetup.repository_ = new FilePersistence::GitRepository{projectsDir};;

	// load newer version
	diffSetup.newVersionManager_ = createTreeManagerFromVersion(diffSetup.repository_, diffSetup.newVersion_);

	// load older version
	diffSetup.oldVersionManager_ = createTreeManagerFromVersion(diffSetup.repository_, diffSetup.oldVersion_);

	Model::Reference::resolvePending();

	return diffSetup;
}

void DiffManager::computeDiff(QString oldVersion, QString newVersion, QList<ChangeWithNodes>& changesWithNodes,
																		  QSet<Model::NodeIdType>& changedNodesToVisualize,
																		  DiffSetup& diffSetup)
{

	diffSetup = initializeDiffPrerequisites(oldVersion, newVersion);

	FilePersistence::Diff diff = diffSetup.repository_->diff(diffSetup.oldVersion_, diffSetup.newVersion_);

	auto changes = diff.changes();


	for (auto change : changes.values())
	{
		// TODO check flags
		if (change->isFake() || change->onlyStructureChange() || change->onlyLabelChange()) continue;

		auto id = change->nodeId();

		auto oldNode = const_cast<Model::Node*>(diffSetup.oldVersionManager_->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(diffSetup.newVersionManager_->nodeIdMap().node(id));

		if (!targetNodeID_.isNull())
		{
			auto targetOldNode = const_cast<Model::Node*>(diffSetup.oldVersionManager_->nodeIdMap().node(targetNodeID_));
			auto targetNewNode = const_cast<Model::Node*>(diffSetup.newVersionManager_->nodeIdMap().node(targetNodeID_));
			if ((targetOldNode && targetOldNode->isSameOrAncestorOf(oldNode))
				 || (targetNewNode && targetNewNode->isSameOrAncestorOf(newNode)))
			{
				changesWithNodes.append({id, oldNode, newNode, change->type()});
			}
			else
				continue;

		} else
			changesWithNodes.append({id, oldNode, newNode, change->type()});

		Model::NodeIdType nodeId;

		// TODO maybe use node instead of id
		if (change->type() != FilePersistence::ChangeType::Deletion)
			if (findChangedNode(diffSetup.newVersionManager_, id, nodeId))
				changedNodesToVisualize.insert(nodeId);
		if (change->type() != FilePersistence::ChangeType::Insertion)
			if (findChangedNode(diffSetup.oldVersionManager_, id, nodeId))
				changedNodesToVisualize.insert(nodeId);
	}

	removeDirectChildrenOfNodesInContainer(changesWithNodes);
	removeNodesWithAncestorPresent(changedNodesToVisualize);
}

void DiffManager::removeNodesWithAncestorPresent(QSet<Model::NodeIdType>& container)
{
	QSet<Model::NodeIdType> nodeIdsToRemove;
	for (auto outerId : container)
	{
		auto outerNode = Model::AllTreeManagers::instance().nodeForId(outerId);
		if (outerNode)
			for (auto innerId : container)
			{
				auto innerNode = Model::AllTreeManagers::instance().nodeForId(innerId);
				if (outerNode->isAncestorOf(innerNode))
					nodeIdsToRemove.insert(innerId);
			}
		else
			nodeIdsToRemove.insert(outerId);
	}

	container.subtract(nodeIdsToRemove);
}

void DiffManager::showDiff(QString oldVersion, QString newVersion)
{
	DiffSetup diffSetup;

	// detailed changes
	QList<ChangeWithNodes> changesWithNodes;

	// contains the nodes which will be drawn
	QSet<Model::NodeIdType> changedNodesToVisualize;

	// fill up lists
	computeDiff(oldVersion, newVersion, changesWithNodes, changedNodesToVisualize, diffSetup);

	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(diffSetup.newVersionManager_);
	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(diffSetup.oldVersionManager_);

	auto diffViewItem = Visualization::VisualizationManager::instance().mainScene()->
			viewItems()->viewItem("DiffView");

	if (diffViewItem)
		Visualization::VisualizationManager::instance().mainScene()->
					viewItems()->removeViewItem(diffViewItem);

	diffViewItem = Visualization::VisualizationManager::instance().mainScene()->
				viewItems()->newViewItem("DiffView");

	diffViewItem->setMajorAxis(Visualization::GridLayouter::NoMajor);
	diffViewItem->setZoomLabelsEnabled(false);

	int row = 0;
	for (auto diffComparisonPair : createDiffComparisonPairs(diffSetup, changedNodesToVisualize))
		diffViewItem->insertNode(diffComparisonPair, {row++, 0});

	// create visualization for changes
	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  [diffViewItem, changesWithNodes, diffSetup]() {
		createOverlaysForChanges(diffViewItem, changesWithNodes);
		auto newCommit = diffSetup.repository_->getCommitInformation(diffSetup.newVersion_);
		QString message = createHTMLCommitInfo(newCommit);
		auto overlay = new Visualization::MessageOverlay{diffViewItem,
				[diffViewItem, message](Visualization::MessageOverlay* overlay)
		{
			auto diffViewItemPos = diffViewItem->scenePos();
			overlay->setPos(diffViewItemPos.x(),
								 diffViewItemPos.y() - overlay->heightInScene());
			auto vDiffComparisonPair = DCast<VersionControlUI::VDiffComparisonPair>(diffViewItem->findVisualizationOf
																					(diffViewItem->nodeAt(Visualization::MajorMinorIndex{})));
			overlay->setScale(vDiffComparisonPair->scaleFactor());
			return message;
		}, Visualization::MessageOverlay::itemStyles().get("info")};


		diffViewItem->addOverlay(overlay, "DiffInfoMessageOverlay");
	});

	// switch to the newly created view
	Visualization::VisualizationManager::instance().mainScene()->viewItems()->switchToView(diffViewItem);
}

QString DiffManager::createHTMLCommitInfo(FilePersistence::CommitMetaData commitMetaData)
{
	return commitMetaData.message_ + "<br/><br/>"
			+ "<font color='gray'>" + commitMetaData.author_.name_ + "</font><br/>"
			+ "<font color='gray'>" + commitMetaData.dateTime_.toString("dd.MM.yyyy hh:mm") + "</font><br/>"
			+ "<font color='gray'>" + commitMetaData.sha1_ + "</font>";
}

void DiffManager::showNodeHistory(Model::NodeIdType targetNodeID, QList<QString> versions)
{
	targetNodeID_ = targetNodeID;

	auto historyViewItem = Visualization::VisualizationManager::instance().mainScene()->
			viewItems()->viewItem("HistoryView");

	if (historyViewItem)
		Visualization::VisualizationManager::instance().mainScene()->
					viewItems()->removeViewItem(historyViewItem);

	historyViewItem = Visualization::VisualizationManager::instance().mainScene()->
				viewItems()->newViewItem("HistoryView");

	historyViewItem->setMajorAxis(Visualization::GridLayouter::NoMajor);
	historyViewItem->setZoomLabelsEnabled(false);

	int col = 0;

	QList<QPair<Model::Node*, QString>> diffComparisonPairInfo;

	for (int i = 1; i < versions.length(); i++)
	{
		DiffSetup diffSetup;

		// detailed changes
		QList<ChangeWithNodes> changesWithNodes;

		// contains the nodes which will be drawn
		QSet<Model::NodeIdType> changedNodesToVisualize;

		QString oldVersion = versions[i-1];
		QString newVersion = versions[i];

		// fill up lists
		computeDiff(oldVersion, newVersion, changesWithNodes, changedNodesToVisualize, diffSetup);

		Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(diffSetup.newVersionManager_);
		Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(diffSetup.oldVersionManager_);

		QString message = createHTMLCommitInfo(diffSetup.repository_->getCommitInformation(diffSetup.newVersion_));

		int row = 0;
		for (auto diffComparisonPair : createDiffComparisonPairs(diffSetup, changedNodesToVisualize))
		{
			historyViewItem->insertNode(diffComparisonPair, {row++, col});
			diffComparisonPairInfo.append({diffComparisonPair, message});
		}

		// create visualization for changes
		Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
									  [historyViewItem, changesWithNodes]() {
			createOverlaysForChanges(historyViewItem, changesWithNodes);
		});
		col++;
	}

	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  [historyViewItem, diffComparisonPairInfo]() {
		for (auto info : diffComparisonPairInfo)
		{
			auto overlay = new Visualization::MessageOverlay{historyViewItem, [historyViewItem, info]
					(Visualization::MessageOverlay* overlay)
			{
				auto vDiffComparisonPair =
						DCast<VersionControlUI::VDiffComparisonPair>(historyViewItem->findVisualizationOf(info.first));
				overlay->setPos(vDiffComparisonPair->scenePos().x(),
									 vDiffComparisonPair->scenePos().y()+vDiffComparisonPair->heightInScene());
				overlay->setScale(vDiffComparisonPair->scaleFactor());
				return info.second;
			}, Visualization::MessageOverlay::itemStyles().get("info") };
			historyViewItem->addOverlay(overlay, "DiffInfoMessageOverlay");
		}
	});


	// switch to the newly created view
	Visualization::VisualizationManager::instance().mainScene()->viewItems()->switchToView(historyViewItem);
}

// TODO maybe better implementation for this
void DiffManager::removeDirectChildrenOfNodesInContainer(QList<ChangeWithNodes>& container)
{
	QHash<Model::Node*, FilePersistence::ChangeType> oldNodes;
	QHash<Model::Node*, FilePersistence::ChangeType> newNodes;

	for (auto change : container)
	{
		if (change.oldNode_)
			oldNodes.insert(change.oldNode_, change.changeType_);
		if (change.newNode_)
			newNodes.insert(change.newNode_, change.changeType_);
	}

	QSet<Model::Node*> oldNodesMarkedForRemoval = findAllNodesWithDirectParentPresent(oldNodes);
	QSet<Model::Node*> newNodesMarkedForRemoval = findAllNodesWithDirectParentPresent(newNodes);

	auto it = container.begin();
	while (it != container.end())
	{
		if (oldNodesMarkedForRemoval.contains(it->oldNode_) || newNodesMarkedForRemoval.contains(it->newNode_))
			it = container.erase(it);
		else
			it++;
	}
}

QSet<Model::Node*> DiffManager::findAllNodesWithDirectParentPresent(QHash<Model::Node*,
																						  FilePersistence::ChangeType>& nodes)
{
	QSet<Model::Node*> result;
	for (auto node : nodes.keys())
		// check that changeType also matches
		if (nodes.contains(node->parent()) && nodes[node] == nodes[node->parent()])
				result.insert(node);

	return result;
}

QSet<Visualization::Item*> DiffManager::findAllItemsWithAncestorsIn(QSet<Visualization::Item*> items)
{
	QSet<Visualization::Item*> result;
	for (auto item : items)
	{
		auto parent = item->parent();
		while (parent)
		{
			if (items.contains(parent))
			{
				result.insert(item);
				break;
			}
			parent = parent->parent();
		}
	}

	return result;
}

// TODO find good way to return and use Node instead of Id
bool DiffManager::findChangedNode(Model::TreeManager* treeManager, Model::NodeIdType id, Model::NodeIdType& resultId)
{
	if (auto node = const_cast<Model::Node*>(treeManager->nodeIdMap().node(id)))
	{

		Model::Node* changedNode = nullptr;

		for (auto contextUnitMatcher : contextUnitMatcherPriorityList_)
			if (auto ancestorNode = node->firstAncestorOfType(contextUnitMatcher))
			{
				changedNode = ancestorNode;

				resultId = treeManager->nodeIdMap().id(changedNode);
				return true;
			}

		resultId = treeManager->nodeIdMap().id(node);

		return true;
	}
	return false;
}

void DiffManager::setOverlayInformationAccordingToChangeType(FilePersistence::ChangeType changeType,
																	QString& highlightOverlayStyle, QString& highlightOverlayName,
																	QString& arrowIconOverlayStyle, QString& arrowIconOverlayName,
																	bool iconsForMoveAndModify)
{
	switch (changeType)
	{
		case FilePersistence::ChangeType::Deletion:
			highlightOverlayName = "delete_highlights";
			highlightOverlayStyle = "delete_frame";
			arrowIconOverlayName = "delete_arrow_icons";
			arrowIconOverlayStyle = "delete_arrow_icon";
			break;
		case FilePersistence::ChangeType::Insertion:
			highlightOverlayName = "insert_highlights";
			highlightOverlayStyle = "insert_frame";
			arrowIconOverlayName = "insert_arrow_icons";
			arrowIconOverlayStyle = "insert_arrow_icon";
			break;
		case FilePersistence::ChangeType::Move:
			highlightOverlayName = "move_highlights";
			highlightOverlayStyle = "move_frame";
			if (iconsForMoveAndModify)
			{
				arrowIconOverlayName = "move_arrow_icons";
				arrowIconOverlayStyle = "move_arrow_icon";
			}
			break;
		case FilePersistence::ChangeType::Stationary:
			highlightOverlayName = "modify_highlights";
			highlightOverlayStyle = "modify_frame";
			if (iconsForMoveAndModify)
			{
				arrowIconOverlayName = "modify_arrow_icons";
				arrowIconOverlayStyle = "modify_arrow_icon";
			}
			break;
		case FilePersistence::ChangeType::Unclassified:
			Q_ASSERT(false);
			break;
	}
}

void DiffManager::createOverlaysForChanges(Visualization::ViewItem* diffViewItem,
														 QList<ChangeWithNodes> changesWithNodes)
{
	static const QString moveArrowLayer = "move_arrows";
	diffViewItem->setArrowStyle(moveArrowLayer, "move_arrow");

	static const QString modifyArrowLayer = "modify_arrows";
	diffViewItem->setArrowStyle(modifyArrowLayer, "modify_arrow");

	QSet<Visualization::Item*> allItemsToScale;
	for (auto change : changesWithNodes)
	{
		QString highlightOverlayStyle;
		QString highlightOverlayName;
		QString arrowIconOverlayStyle;
		QString arrowIconOverlayName;

		setOverlayInformationAccordingToChangeType(change.changeType_, highlightOverlayStyle, highlightOverlayName,
												  arrowIconOverlayStyle, arrowIconOverlayName);


		Visualization::Item* oldNodeItem = addOverlaysAndReturnItem(change.oldNode_, diffViewItem,
																						highlightOverlayName, highlightOverlayStyle,
																						arrowIconOverlayName, arrowIconOverlayStyle);
		Visualization::Item* newNodeItem = addOverlaysAndReturnItem(change.newNode_, diffViewItem,
																						highlightOverlayName, highlightOverlayStyle,
																						arrowIconOverlayName, arrowIconOverlayStyle);

		if (oldNodeItem)
			allItemsToScale.insert(oldNodeItem);

		if (newNodeItem)
			allItemsToScale.insert(newNodeItem);


		if (newNodeItem && oldNodeItem)
		{
			if (change.changeType_== FilePersistence::ChangeType::Move)
			{
				auto overlay = new Visualization::ArrowOverlay{oldNodeItem, newNodeItem,
						  Visualization::ArrowOverlay::itemStyles().get("move_arrow")};
				diffViewItem->addOverlay(overlay, moveArrowLayer);
			} else if (change.changeType_ == FilePersistence::ChangeType::Stationary)
			{
				auto overlay = new Visualization::ArrowOverlay{oldNodeItem, newNodeItem,
							Visualization::ArrowOverlay::itemStyles().get("modify_arrow")};
				diffViewItem->addOverlay(overlay, modifyArrowLayer);
			}
		}

	}

	QSet<Visualization::Item*> removeItems = findAllItemsWithAncestorsIn(allItemsToScale);

	allItemsToScale.subtract(removeItems);

	Visualization::VisualizationManager::instance().mainScene()->
			addOnZoomHandler([allItemsToScale](qreal factor)
	{
		for (auto item : allItemsToScale)
		{
			if (factor >= 1.0)
				item->setScale(1.0);
			else if (factor >= 0.05)
				item->setScale((1/factor));
			else
				item->setScale((1/factor) * std::pow(0.95, 1/factor));
		}
	});

	// set zoom level further out and center the scene
	Visualization::VisualizationManager::instance().mainView()->zoom(7);
	auto centerTop = Visualization::VisualizationManager::instance().mainScene()->
			currentViewItem()->boundingRect().center();
	centerTop.setY(Visualization::VisualizationManager::instance().mainScene()->
						currentViewItem()->boundingRect().top());
	Visualization::VisualizationManager::instance().mainView()->centerOn(centerTop);
}

Visualization::Item* DiffManager::addOverlaysAndReturnItem(Model::Node* node, Visualization::ViewItem* viewItem,
																QString highlightOverlayName, QString highlightOverlayStyle,
																QString arrowIconOverlayName, QString arrowIconOverlayStyle)
{
	Visualization::Item* resultItem = nullptr;

	if (node)
	{
		if ((resultItem = viewItem->findVisualizationOf(node)))
		{
			auto overlay = new Visualization::HighlightOverlay{resultItem,
					Visualization::HighlightOverlay::itemStyles().get(highlightOverlayStyle)};
			resultItem->addOverlay(overlay, highlightOverlayName);

			if (!arrowIconOverlayName.isNull())
			{
				auto iconOverlay = new Visualization::IconOverlay{resultItem,
						Visualization::IconOverlay::itemStyles().get(arrowIconOverlayStyle)};
				resultItem->addOverlay(iconOverlay, arrowIconOverlayName);
			}

		}
		else if (auto parent = DCast<Model::CompositeNode>(node->parent()))
		{
			auto index = parent->indexOf(node);
			if (!parent->meta().attribute(index).name().startsWith("_"))
				return addOverlaysAndReturnItem(node->parent(), viewItem, highlightOverlayName, highlightOverlayStyle,
														  arrowIconOverlayName, arrowIconOverlayStyle);
		}
	}
	return resultItem;
}


QList<DiffComparisonPair*> DiffManager::createDiffComparisonPairs(DiffSetup& diffSetup,
													 QSet<Model::NodeIdType> diffComparisonPairNodeIds)
{
	QList<DiffComparisonPair*> diffComparisonPairs;
	for (auto id : diffComparisonPairNodeIds)
	{
		auto oldNode = const_cast<Model::Node*>(diffSetup.oldVersionManager_->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(diffSetup.newVersionManager_->nodeIdMap().node(id));

		auto diffNode = new DiffComparisonPair{oldNode, newNode};
		diffComparisonPairs.append(diffNode);
	}

	std::sort(diffComparisonPairs.begin(), diffComparisonPairs.end(), [](DiffComparisonPair* a, DiffComparisonPair* b) {
			auto aComparisonName = a->comparisonName();
			auto bComparisonName = b->comparisonName();
			return aComparisonName < bComparisonName;
		 });

	return diffComparisonPairs;
}

Model::TreeManager* DiffManager::createTreeManagerFromVersion(FilePersistence::GitRepository* repository,
																				  QString version)
{
	if (version == FilePersistence::GitRepository::WORKDIR)
	{
		auto fileStore = new FilePersistence::SimpleTextFileStore {"projects/"};
		Model::TreeManager* versionTreeManager = new Model::TreeManager{};
		versionTreeManager->load(fileStore, project_, false);

		return versionTreeManager;

	}

	std::unique_ptr<const FilePersistence::Commit> commit{repository->getCommit(version)};

	auto fileStore = new FilePersistence::SimpleTextFileStore {
				[this, &commit](QString filename, const char*& data, int& size)
				{ return commit->getFileContent(filename, data, size, false); }
			};

	Model::TreeManager* versionTreeManager = new Model::TreeManager{};
	versionTreeManager->load(fileStore, project_, false);

	return versionTreeManager;
}

}
