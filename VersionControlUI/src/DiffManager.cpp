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

#include "nodes/DiffFrame.h"
#include "items/VDiffFrame.h"

namespace VersionControlUI
{

const QString DiffManager::OVERVIEW_HIGHLIGHT_OVERLAY_NAME = "changeOverviewHighlights";
const QString DiffManager::OVERVIEW_ICON_OVERLAY_NAME = "changeOverviewIcons";
const QString DiffManager::NAME_CHANGE_OVERLAY_NAME = "nameChange_overlay";
const QString DiffManager::NAME_CHANGE_ARROW_OVERLAY_NAME = "nameChange_arrow_overlay";
QHash<Model::NodeIdType, bool> DiffManager::nameChangesIdsIsNameText_;
QList<ChangeWithNodes> DiffManager::nameChanges_;
DiffManager::NameChangeVisualizationFlags DiffManager::nameChangeVisualizationFlags_{Summary | NameText | References};
QList<int> DiffManager::nameChangeOnZoomHandlerIds_;
QHash<Visualization::ViewItem*, int> DiffManager::onZoomHandlerIdPerViewItem_;
QSet<Visualization::Item*> DiffManager::nameChangesScaledByAncestor_;

DiffManager::DiffManager(QString project, QList<Model::SymbolMatcher> contextUnitMatcherPriorityList,
								 Model::NodeIdType targetNodeID)
	: project_{project}, contextUnitMatcherPriorityList_{contextUnitMatcherPriorityList}, targetNodeId_{targetNodeID}
{
}

DiffSetup DiffManager::initializeDiffPrerequisites(QString oldVersion, QString newVersion)
{
	QString projectsDir = "projects/" + project_;

	// get GitRepository
	if (!FilePersistence::GitRepository::repositoryExists(projectsDir))
		throw VersionControlUIException{"Diff setup not possible. No repository found at " + projectsDir};

	DiffSetup diffSetup{};

	diffSetup.managerName_ = project_;

	diffSetup.oldVersion_ = oldVersion;
	diffSetup.newVersion_ = newVersion;

	diffSetup.repository_ = new FilePersistence::GitRepository{projectsDir};;

	// load newer version
	diffSetup.newVersionManager_ = createTreeManagerFromVersion(diffSetup.repository_, diffSetup.newVersion_);
	diffSetup.newVersionManager_->setRevisionName("new");

	// load older version
	diffSetup.oldVersionManager_ = createTreeManagerFromVersion(diffSetup.repository_, diffSetup.oldVersion_);
	diffSetup.oldVersionManager_->setRevisionName("old");

	Model::Reference::resolvePending();

	return diffSetup;
}

void DiffManager::clear()
{
	auto currentViewItem = Visualization::VisualizationManager::instance().
			mainScene()->currentViewItem();

	// remove overlays
	currentViewItem->scene()->removeOverlayGroup(OVERVIEW_HIGHLIGHT_OVERLAY_NAME);
	currentViewItem->scene()->removeOverlayGroup(OVERVIEW_ICON_OVERLAY_NAME);

	auto iter = onZoomHandlerIdPerViewItem_.find(currentViewItem);
	if (iter != onZoomHandlerIdPerViewItem_.end())
	{
		Visualization::VisualizationManager::instance().mainScene()
				->removeOnZoomHandler(*iter);
		onZoomHandlerIdPerViewItem_.erase(iter);
	}
}

QString DiffManager::computeNameChangeInformation(const DiffSetup& diffSetup)
{
	if (nameChangeInformation_.isEmpty() || !nameChangeVisualizationFlags_.testFlag(Summary))
		return "";

	QString nameChangeInformation = "";

	for (auto entry : nameChangeInformation_)
	{
		auto id = entry.second;

		auto oldNode = const_cast<Model::Node*>(diffSetup.oldVersionManager_->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(diffSetup.newVersionManager_->nodeIdMap().node(id));

		// if targetNode is defined, we only want to know about name changes related to it
		if (!areInTargetNodeSubtree(oldNode, newNode, diffSetup))
			 continue;

		auto oldNodeObjectPathList = DiffFrame::computeObjectPath(oldNode).split(".");
		auto newNodeObjectPathList = DiffFrame::computeObjectPath(newNode).split(".");

		// remove project name if it didn't change
		if (oldNodeObjectPathList.first() == newNodeObjectPathList.first())
		{
			oldNodeObjectPathList.removeFirst();
			newNodeObjectPathList.removeFirst();
		}

		nameChangeInformation += oldNodeObjectPathList.join(".") +
				// html entity for right arrow
				" &rarr; "
				+ newNodeObjectPathList.join(".") + "<br/>";
	}

	// if there is a message to show add header
	if (!nameChangeInformation.isEmpty())
		nameChangeInformation = nameChangeInformation.prepend("<b>Renamings</b>:<br/>");

	return nameChangeInformation;

}

bool DiffManager::processNameChange(Model::Node* oldNode, Model::Node* newNode, const DiffSetup& diffSetup) {
	if (!oldNode || !newNode)
		return false;

	if (auto oldReference = DCast<Model::Reference>(oldNode))
		if (auto newReference = DCast<Model::Reference>(newNode))
		{
			auto result = nameChangeInformation_.find(oldReference->name());
			if (result != nameChangeInformation_.end() && result->first == newReference->name())
			{
				if (oldReference->target() && newReference->target())
				{
					auto oldRefId = diffSetup.oldVersionManager_->nodeIdMap().idIfExists(oldReference->target());
					auto newRefId = diffSetup.newVersionManager_->nodeIdMap().idIfExists(newReference->target());

					return (oldRefId == newRefId) && (newRefId == result->second);
				}
				return true;
			}
			return false;
		}

	return false;

}

VersionNodes DiffManager::retrieveVersionNodesForId(const Model::NodeIdType& id, const DiffSetup& diffSetup)
{
	auto oldNode = const_cast<Model::Node*>(diffSetup.oldVersionManager_->nodeIdMap().node(id));
	auto newNode = const_cast<Model::Node*>(diffSetup.newVersionManager_->nodeIdMap().node(id));

	return {oldNode, newNode};
}

void DiffManager::processNameTextChanges(FilePersistence::IdToChangeDescriptionHash& changes,
													  const DiffSetup& diffSetup)
{
	auto iter = changes.begin();
	while (iter != changes.end())
	{
		auto id = iter->get()->nodeId();
		auto versionNodes = retrieveVersionNodesForId(id, diffSetup);

		if (auto oldNameText = DCast<Model::NameText>(versionNodes.oldNode_))
			if (auto newNameText = DCast<Model::NameText>(versionNodes.newNode_))
			{
				auto parentId = diffSetup.newVersionManager_->nodeIdMap().idIfExists(newNameText->parent());
				Q_ASSERT(!parentId.isNull());

				nameChangeInformation_.insert(oldNameText->get(), {newNameText->get(), parentId});
				nameChangesIdsIsNameText_.insert(id, true);
				nameChanges_.append({id, versionNodes, iter->get()->type()});
			}
		iter++;
	}
}

bool DiffManager::areInTargetNodeSubtree(Model::Node* oldNode, Model::Node* newNode,
																  const DiffSetup& diffSetup)
{
	if (targetNodeId_.isNull())
		return true;

	auto targetOldNode = const_cast<Model::Node*>(diffSetup.oldVersionManager_->nodeIdMap().node(targetNodeId_));
	auto targetNewNode = const_cast<Model::Node*>(diffSetup.newVersionManager_->nodeIdMap().node(targetNodeId_));

	if ((targetOldNode && targetOldNode->isSameOrAncestorOf(oldNode))
		 || (targetNewNode && targetNewNode->isSameOrAncestorOf(newNode)))
		return true;

	return false;
}

bool DiffManager::shouldShowChange(Model::NodeIdType id)
{
	auto iter = nameChangesIdsIsNameText_.find(id);

	// id is not involved in name change
	if (iter == nameChangesIdsIsNameText_.end())
		return true;

	// node for id is NameText, if NameText flag is not set don't show it
	if (iter.value() && !nameChangeVisualizationFlags_.testFlag(NameText))
		return false;

	// node for id is Reference, if References flag is not set don't show it
	if (!iter.value() && !nameChangeVisualizationFlags_.testFlag(References))
		return false;

	return true;
}


void DiffManager::computeDiff(QString oldVersion, QString newVersion, QList<ChangeWithNodes>& changesWithNodes,
																		  QSet<Model::NodeIdType>& changedNodesToVisualize,
																		  DiffSetup& diffSetup)
{
	diffSetup = initializeDiffPrerequisites(oldVersion, newVersion);

	FilePersistence::Diff diff = diffSetup.repository_->diff(diffSetup.oldVersion_, diffSetup.newVersion_);

	auto changes = diff.changes();

	processNameTextChanges(changes, diffSetup);

	for (auto change : changes.values())
	{
		// TODO check flags
		if (change->isFake() || change->onlyStructureChange() || change->onlyLabelChange()) continue;

		auto id = change->nodeId();

		auto versionNodes = retrieveVersionNodesForId(id, diffSetup);

		// don't process this change further if it is a name change, they will be shown seperately
		if (processNameChange(versionNodes.oldNode_, versionNodes.newNode_, diffSetup))
		{
			nameChangesIdsIsNameText_.insert(id, false);
			nameChanges_.append({id, versionNodes, change->type()});
		}

		if (shouldShowChange(id))
		{
			if (areInTargetNodeSubtree(versionNodes.oldNode_, versionNodes.newNode_, diffSetup))
				changesWithNodes.append({id, versionNodes, change->type()});
			else
				continue;

			Model::NodeIdType nodeId;

			if (change->type() != FilePersistence::ChangeType::Deletion)
				if (findChangedNode(diffSetup.newVersionManager_, id, nodeId))
					changedNodesToVisualize.insert(nodeId);
			if (change->type() != FilePersistence::ChangeType::Insertion)
				if (findChangedNode(diffSetup.oldVersionManager_, id, nodeId))
					changedNodesToVisualize.insert(nodeId);
		}
	}

	removeDirectChildrenOfNodesInContainer(changesWithNodes);
	removeNodesWithAncestorPresent(changedNodesToVisualize);
}


bool DiffManager::toggleNameChangeHighlights(Visualization::Item* target,
																QKeySequence, Interaction::ActionRegistry::InputState)
{

	auto currentViewItem = target->scene()->currentViewItem();

	for (int id : nameChangeOnZoomHandlerIds_)
		Visualization::VisualizationManager::instance().mainScene()->removeOnZoomHandler(id);

	nameChangeOnZoomHandlerIds_.clear();

	currentViewItem->scene()->removeOverlayGroup(NAME_CHANGE_OVERLAY_NAME);
	currentViewItem->scene()->removeOverlayGroup(NAME_CHANGE_ARROW_OVERLAY_NAME);

	QList<ChangeWithNodes> nameChangesToShow;

	if (nameChangeVisualizationFlags_.testFlag(NameText) && nameChangeVisualizationFlags_.testFlag(References))
	{
		nameChangeVisualizationFlags_ &= ~NameText;
		nameChangeVisualizationFlags_ &= ~References;
	}
	else if (!nameChangeVisualizationFlags_.testFlag(NameText) && !nameChangeVisualizationFlags_.testFlag(References))
	{
		nameChangeVisualizationFlags_ |= NameText;
	}
	else if (nameChangeVisualizationFlags_.testFlag(NameText) && !nameChangeVisualizationFlags_.testFlag(References))
	{
		nameChangeVisualizationFlags_ |= References;
	}
	else if (!nameChangeVisualizationFlags_.testFlag(NameText) && nameChangeVisualizationFlags_.testFlag(References))
	{
		nameChangeVisualizationFlags_ |= NameText;
	}

	for (auto change : nameChanges_)
		if (shouldShowChange(change.id_))
			nameChangesToShow.append(change);

	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  [currentViewItem, nameChangesToShow]() {
		createOverlaysForChanges(currentViewItem, nameChangesToShow, nameChangesIdsIsNameText_.keys());
	});

	return true;

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

void DiffManager::highlightChangedParts(QString oldVersion, QString newVersion, Model::TreeManager* manager)
{
	DiffSetup diffSetup;

	// detailed changes
	QList<ChangeWithNodes> changesWithNodes;

	// contains the nodes which will be drawn
	QSet<Model::NodeIdType> changedNodesToVisualize;

	// fill up lists
	computeDiff(oldVersion, newVersion, changesWithNodes, changedNodesToVisualize, diffSetup);

	// make sure any old overlays are removed first
	clear();

	QString highlightOverlayStyle;
	QString highlightOverlayName;
	QString arrowIconOverlayStyle;
	QString arrowIconOverlayName;

	QSet<Visualization::Item*> itemsToScale;

	auto currentViewItem = Visualization::VisualizationManager::instance().
			mainScene()->currentViewItem();

	for (auto changeWithNode : changesWithNodes)
	{
		auto node = const_cast<Model::Node*>(manager->nodeIdMap().node(changeWithNode.id_));

		if (node)
		{
			setOverlayInformationAccordingToChangeType(changeWithNode.changeType_, highlightOverlayStyle, highlightOverlayName,
																	 arrowIconOverlayStyle, arrowIconOverlayName, true);

			highlightOverlayName = OVERVIEW_HIGHLIGHT_OVERLAY_NAME;
			arrowIconOverlayName = OVERVIEW_ICON_OVERLAY_NAME;

			auto item = addOverlaysAndReturnItem(node, currentViewItem,
															 highlightOverlayName, highlightOverlayStyle,
															 arrowIconOverlayName, arrowIconOverlayStyle);
			if (item) itemsToScale.insert(item);
		}
	}

	scaleItems(itemsToScale, currentViewItem);

}


void DiffManager::showDiff(QString oldVersion, QString newVersion)
{

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
	auto diffFramesAndSetup = computeDiffFramesAndOverlays(oldVersion, newVersion, diffViewItem);
	auto diffFrames = diffFramesAndSetup.diffFrames_;
	for (auto diffFrame : diffFrames)
		diffViewItem->insertNode(diffFrame, {row++, 0});

	// switch to the newly created view
	Visualization::VisualizationManager::instance().mainScene()->viewItems()->switchToView(diffViewItem);

	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  []() { Visualization::MainView::centerAndZoomViewToFitEntireScene();});
}

void DiffManager::createOverlaysForChanges(QList<ChangeWithNodes> changesWithNodes, Visualization::ViewItem* viewItem,
														 DiffSetup diffSetup, Visualization::Item* anchorItem)
{
	auto nameChangeInfo = computeNameChangeInformation(diffSetup);

	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  [viewItem, changesWithNodes, diffSetup, nameChangeInfo, anchorItem]() {
		createOverlaysForChanges(viewItem, changesWithNodes, nameChangesIdsIsNameText_.keys());
		auto message = createHTMLCommitInfo(diffSetup.repository_, diffSetup.newVersion_);
		if (!nameChangeInfo.isEmpty()) message += "<br/><br/>" + nameChangeInfo;
		auto overlay = new Visualization::MessageOverlay{viewItem,
				[viewItem, message, anchorItem](Visualization::MessageOverlay* overlay)
		{
			overlay->setPos(anchorItem->scenePos().x(),
								 anchorItem->scenePos().y()-overlay->heightInScene());
			return message;
		}, Visualization::MessageOverlay::itemStyles().get("info"), true};


		viewItem->addOverlay(overlay, "DiffInfoMessageOverlay");
	});
}

DiffFramesAndSetup DiffManager::computeDiffFramesAndOverlays(QString oldVersion, QString newVersion,
																										Visualization::ViewItem* viewItem)
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

	auto diffFrames = createDiffFrames(diffSetup, changedNodesToVisualize, changesWithNodes);

	createOverlaysForChanges(changesWithNodes, viewItem, diffSetup, viewItem);

	return {diffFrames, diffSetup};
}

QString DiffManager::createHTMLCommitInfo(const FilePersistence::GitRepository* repository, QString revision)
{
	if (revision == FilePersistence::GitRepository::WORKDIR)
		return "Comparing against working directory";

	auto commitMetaData = repository->getCommitInformation(revision);
	bool messageEndsInNewline = commitMetaData.message_.endsWith("\n");

	return commitMetaData.message_.replace("\n", "<br/>") + (messageEndsInNewline ? "<br/>" : "<br/><br/>")
			+ "<font color='gray'>" + commitMetaData.author_.name_ + "</font><br/>"
			+ "<font color='gray'>" + commitMetaData.dateTime_.toString("dd.MM.yyyy hh:mm") + "</font><br/>"
			+ "<font color='gray'>" + commitMetaData.sha1_.left(15) + "</font>";
}

void DiffManager::showNodeHistory(Model::NodeIdType targetNodeID, QList<QString> versions)
{
	targetNodeId_ = targetNodeID;

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

	QList<QPair<Model::Node*, QString>> diffFrameInfo;

	QHash<QString, QPair<QString, Model::NodeIdType>> tempNameChangeInformation = {nameChangeInformation_};
	QHash<Model::NodeIdType, bool> tempNameChangesIdsIsNameText = {nameChangesIdsIsNameText_};

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

		auto message = createHTMLCommitInfo(diffSetup.repository_, diffSetup.newVersion_);

		auto diffFrames = createDiffFrames(diffSetup, changedNodesToVisualize, changesWithNodes);

		// if summary activated and no changes to show, insert dummy DiffFrame
		if (nameChangeVisualizationFlags_.testFlag(Summary) && !nameChangesIdsIsNameText_.isEmpty()
			 && diffFrames.isEmpty())
			diffFrames.append(new DiffFrame{});

		int row = 0;
		for (auto diffFrame : diffFrames)
			historyViewItem->insertNode(diffFrame, {row++, col});

		if (nameChangeVisualizationFlags_.testFlag(Summary) && !nameChangesIdsIsNameText_.isEmpty())
			message += "<br/><br/>" + computeNameChangeInformation(diffSetup);

		if (!diffFrames.isEmpty())
			diffFrameInfo.append({diffFrames.first(), message});

		auto nameChanges = nameChangesIdsIsNameText_.keys();

		// create visualization for changes
		Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
									  [historyViewItem, changesWithNodes, nameChanges]() {
			createOverlaysForChanges(historyViewItem, changesWithNodes, nameChanges);
		});
		col++;

		tempNameChangeInformation.unite(nameChangeInformation_);
		tempNameChangesIdsIsNameText.unite(nameChangesIdsIsNameText_);
		nameChangeInformation_.clear();
		nameChangesIdsIsNameText_.clear();
	}

	// restore all information
	nameChangeInformation_.unite(tempNameChangeInformation);
	nameChangesIdsIsNameText_.unite(tempNameChangesIdsIsNameText);

	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  [historyViewItem, diffFrameInfo]() {
		for (auto info : diffFrameInfo)
		{
			auto overlay = new Visualization::MessageOverlay{historyViewItem, [historyViewItem, info]
					(Visualization::MessageOverlay* overlay)
			{
				auto vDiffFrame =
						DCast<VersionControlUI::VDiffFrame>(historyViewItem->findVisualizationOf(info.first));
				overlay->setPos(vDiffFrame->scenePos().x(),
									 vDiffFrame->scenePos().y()-overlay->heightInScene());
				return info.second;
			}, Visualization::MessageOverlay::itemStyles().get("info"), true };
			historyViewItem->addOverlay(overlay, "DiffInfoMessageOverlay");
		}
	});

	// switch to the newly created view
	Visualization::VisualizationManager::instance().mainScene()->viewItems()->switchToView(historyViewItem);

	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  []() { Visualization::MainView::centerAndZoomViewToFitEntireScene();});
}

// TODO maybe better implementation for this
void DiffManager::removeDirectChildrenOfNodesInContainer(QList<ChangeWithNodes>& container)
{
	QHash<Model::Node*, FilePersistence::ChangeType> oldNodes;
	QHash<Model::Node*, FilePersistence::ChangeType> newNodes;

	for (auto change : container)
	{
		if (change.versionNodes_.oldNode_)
			oldNodes.insert(change.versionNodes_.oldNode_, change.changeType_);
		if (change.versionNodes_.newNode_)
			newNodes.insert(change.versionNodes_.newNode_, change.changeType_);
	}

	QSet<Model::Node*> oldNodesMarkedForRemoval = findAllNodesWithDirectParentPresent(oldNodes);
	QSet<Model::Node*> newNodesMarkedForRemoval = findAllNodesWithDirectParentPresent(newNodes);

	auto it = container.begin();
	while (it != container.end())
	{
		if (oldNodesMarkedForRemoval.contains(it->versionNodes_.oldNode_)
			 || newNodesMarkedForRemoval.contains(it->versionNodes_.newNode_))
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
	return findAllItemsWithAncestorsIn(items, items);
}


QSet<Visualization::Item*> DiffManager::findAllItemsWithAncestorsIn(QSet<Visualization::Item*> items,
																						  QSet<Visualization::Item*> possibleAncestors)
{
	QSet<Visualization::Item*> result;
	for (auto item : items)
	{
		auto parent = item->parent();
		while (parent)
		{
			if (possibleAncestors.contains(parent))
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
	highlightOverlayStyle = QString{};
	highlightOverlayName = QString{};
	arrowIconOverlayStyle = QString{};
	arrowIconOverlayName = QString{};

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

void DiffManager::scaleItems(QSet<Visualization::Item*> itemsToScale, Visualization::ViewItem* currentViewItem,
									  bool nameChangeRelated)
{
	QSet<Visualization::Item*> removeItems = findAllItemsWithAncestorsIn(itemsToScale);

	itemsToScale.subtract(removeItems);

	auto id = Visualization::VisualizationManager::instance().mainScene()->
			addOnZoomHandler([itemsToScale](qreal factor)
	{
		for (auto item : itemsToScale)
		{
			if (factor >= 1.0)
				item->setScale(1.0);
			else if (factor >= 0.015)
				item->setScale((1/factor));
			else
				item->setScale((1/factor) * std::pow(0.95, 1/factor));
		}
	},
	[itemsToScale]()
		{
			for (auto item : itemsToScale)
				item->setScale(1.0);
		});

	if (nameChangeRelated)
		nameChangeOnZoomHandlerIds_.append(id);
	else
		onZoomHandlerIdPerViewItem_.insert(currentViewItem, id);

}

void DiffManager::createOverlaysForChanges(Visualization::ViewItem* diffViewItem,
														 QList<ChangeWithNodes> changesWithNodes,
														 QList<Model::NodeIdType> nameChangesIds)
{
	static const QString moveArrowLayer = "move_arrows";
	diffViewItem->setArrowStyle(moveArrowLayer, "move_arrow");

	static const QString modifyArrowLayer = "modify_arrows";
	diffViewItem->setArrowStyle(modifyArrowLayer, "modify_arrow");

	static const QString nameChangeArrowLayer = "nameChange_arrows";
	diffViewItem->setArrowStyle(nameChangeArrowLayer, "modify_arrow");

	QSet<Visualization::Item*> itemsToScale;
	QSet<Visualization::Item*> nameChangeItemsToScale;

	for (auto change : changesWithNodes)
	{
		QString highlightOverlayStyle;
		QString highlightOverlayName;
		QString arrowIconOverlayStyle;
		QString arrowIconOverlayName;

		bool isNameChangeRelated = nameChangesIds.contains(change.id_);

		setOverlayInformationAccordingToChangeType(change.changeType_, highlightOverlayStyle, highlightOverlayName,
												  arrowIconOverlayStyle, arrowIconOverlayName);

		if (isNameChangeRelated)
			highlightOverlayName = NAME_CHANGE_OVERLAY_NAME;


		Visualization::Item* oldNodeItem = addOverlaysAndReturnItem(change.versionNodes_.oldNode_, diffViewItem,
																						highlightOverlayName, highlightOverlayStyle,
																						arrowIconOverlayName, arrowIconOverlayStyle);
		Visualization::Item* newNodeItem = addOverlaysAndReturnItem(change.versionNodes_.newNode_, diffViewItem,
																						highlightOverlayName, highlightOverlayStyle,
																						arrowIconOverlayName, arrowIconOverlayStyle);
		if (oldNodeItem)
		{
			if (isNameChangeRelated)
				nameChangeItemsToScale.insert(oldNodeItem);
			else
				itemsToScale.insert(oldNodeItem);
		}

		if (newNodeItem)
		{
			if (isNameChangeRelated)
				nameChangeItemsToScale.insert(newNodeItem);
			else
				itemsToScale.insert(newNodeItem);
		}

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
				if (isNameChangeRelated)
					diffViewItem->addOverlay(overlay, NAME_CHANGE_ARROW_OVERLAY_NAME);
				else
					diffViewItem->addOverlay(overlay, modifyArrowLayer);
			}
		}

	}

	if (!itemsToScale.isEmpty())
	{
		scaleItems(itemsToScale, diffViewItem);

		nameChangesScaledByAncestor_ += findAllItemsWithAncestorsIn(nameChangeItemsToScale, itemsToScale);

	}

	if (!nameChangeItemsToScale.isEmpty())
	{
		nameChangeItemsToScale.subtract(nameChangesScaledByAncestor_);
		scaleItems(nameChangeItemsToScale, diffViewItem, true);
	}

	diffViewItem->setUpdateNeeded(Visualization::Item::StandardUpdate);
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


QList<DiffFrame*> DiffManager::createDiffFrames(DiffSetup& diffSetup,
													 QSet<Model::NodeIdType> diffFrameNodeIds,
													 const QList<ChangeWithNodes>& changesWithNodes)
{
	QList<DiffFrame*> diffFrames;
	for (auto id : diffFrameNodeIds)
	{
		auto oldNode = const_cast<Model::Node*>(diffSetup.oldVersionManager_->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(diffSetup.newVersionManager_->nodeIdMap().node(id));

		auto diffNode = new DiffFrame{oldNode, newNode};

		for (auto changeWithNode : changesWithNodes)
		{
			if (changeWithNode.versionNodes_.oldNode_ && oldNode->isSameOrAncestorOf(changeWithNode.versionNodes_.oldNode_))
				diffNode->addOldChangedNode(changeWithNode.versionNodes_.oldNode_);

			if (changeWithNode.versionNodes_.newNode_ && newNode->isSameOrAncestorOf(changeWithNode.versionNodes_.newNode_))
				diffNode->addNewChangedNode(changeWithNode.versionNodes_.newNode_);
		}

		diffFrames.append(diffNode);
	}

	std::sort(diffFrames.begin(), diffFrames.end(), [](DiffFrame* a, DiffFrame* b) {
			auto aComparisonName = a->comparisonName();
			auto bComparisonName = b->comparisonName();
			return aComparisonName < bComparisonName;
		 });

	return diffFrames;
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
