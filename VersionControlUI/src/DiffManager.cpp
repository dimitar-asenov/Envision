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

#include "FilePersistence/src/version_control/GitRepository.h"
#include "FilePersistence/src/simple/SimpleTextFileStore.h"

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/expressions/EmptyExpression.h"

#include "ModelBase/src/model/AllTreeManagers.h"
#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Reference.h"

namespace VersionControlUI
{

struct ChangeWithNodes {
		Model::Node* oldNode_{};
		Model::Node* newNode_{};
		FilePersistence::ChangeType changeType_{FilePersistence::ChangeType::Unclassified};
};

DiffManager::DiffManager(QString oldVersion, QString newVersion, QString project,
								 Model::SymbolMatcher contextUnitMatcher) :
	oldVersion_{oldVersion}, newVersion_{newVersion}, project_{project}, contextUnitMatcher_{contextUnitMatcher}
{}

void DiffManager::visualize()
{
	QString projectsDir = "projects/" + project_;

	// get GitRepository
	if (!FilePersistence::GitRepository::repositoryExists(projectsDir))
	{
		qDebug() << "no repository found at " + projectsDir;
		return;
	}

	FilePersistence::GitRepository repository{projectsDir};

	// load newer version
	Model::TreeManager* newVersionManager = createTreeManagerFromVersion(repository, newVersion_);

	// load older version
	Model::TreeManager* oldVersionManager = createTreeManagerFromVersion(repository, oldVersion_);

	Model::Reference::resolvePending();

	FilePersistence::Diff diff = repository.diff(oldVersion_, newVersion_);

	auto changes = diff.changes();

	// detailed changes
	QList<ChangeWithNodes> changesWithNodes;

	// contains the nodes which will be drawn
	QSet<Model::NodeIdType> changedNodesToVisualize;

	for (auto change : changes.values())
	{
		// TODO check flags
		if (change->isFake() || change->onlyStructureChange()) continue;

		auto id = change->nodeId();

		changesWithNodes.append({const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id)),
														 const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id)),
														 change->type()});

		Model::NodeIdType nodeId;

		// TODO maybe use node instead of id
		if (change->type() != FilePersistence::ChangeType::Deletion)
			if (findChangedNode(newVersionManager, id, nodeId))
				changedNodesToVisualize.insert(nodeId);
		if (change->type() != FilePersistence::ChangeType::Insertion)
			if (findChangedNode(oldVersionManager, id, nodeId))
				changedNodesToVisualize.insert(nodeId);
	}

	removeDirectChildrenOfNodesInContainer(&changesWithNodes);

	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(newVersionManager);
	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(oldVersionManager);

	auto diffViewItem = Visualization::VisualizationManager::instance().mainScene()->
			viewItems()->newViewItem("DiffView");

	diffViewItem->setMajorAxis(Visualization::GridLayouter::NoMajor);

	// add nodes to visualization, if not avaible show text
	visualizeChangedNodes(oldVersionManager, changedNodesToVisualize, newVersionManager, diffViewItem);

	// create visualization for changes
	Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								  [diffViewItem, changesWithNodes]() {
		createOverlaysForChanges(diffViewItem, changesWithNodes);
	});

	// switch to the newly created view
	Visualization::VisualizationManager::instance().mainScene()->viewItems()->switchToView(diffViewItem);
}

// TODO maybe better implementation for this
void DiffManager::removeDirectChildrenOfNodesInContainer(QList<ChangeWithNodes>* container)
{
	QHash<Model::Node*, FilePersistence::ChangeType> oldNodes;
	QHash<Model::Node*, FilePersistence::ChangeType> newNodes;

	for (auto change : *container)
	{
		if (change.oldNode_)
			oldNodes.insert(change.oldNode_, change.changeType_);
		if (change.newNode_)
			newNodes.insert(change.newNode_, change.changeType_);
	}

	QSet<Model::Node*> oldNodesMarkedForRemoval = findAllNodesWithDirectParentPresent(oldNodes);
	QSet<Model::Node*> newNodesMarkedForRemoval = findAllNodesWithDirectParentPresent(newNodes);

	auto it = container->begin();
	while (it != container->end())
	{
		if (oldNodesMarkedForRemoval.contains(it->oldNode_) || newNodesMarkedForRemoval.contains(it->newNode_))
			it = container->erase(it);
		else
			it++;
	}
}

QSet<Model::Node*> DiffManager::findAllNodesWithDirectParentPresent(QHash<Model::Node*,
																						  FilePersistence::ChangeType>& nodes)
{
	QSet<Model::Node*> nodesMarkedForRemoval;
	for (auto node : nodes.keys())
		// check that changeType also matches
		if (nodes.contains(node->parent()) && nodes[node] == nodes[node->parent()])
				nodesMarkedForRemoval.insert(node);

	return nodesMarkedForRemoval;
}

// TODO find good way to return and use Node instead of Id
bool DiffManager::findChangedNode(Model::TreeManager* treeManager, Model::NodeIdType id, Model::NodeIdType& resultId)
{
	if (auto node = const_cast<Model::Node*>(treeManager->nodeIdMap().node(id)))
	{

		Model::Node* changedNode = nullptr;

		if (auto ancestorNode = node->firstAncestorOfType(contextUnitMatcher_))
		{
			changedNode = ancestorNode;

			resultId = treeManager->nodeIdMap().id(changedNode);
			return true;
		}
	}
	return false;
}

void DiffManager::createOverlaysForChanges(Visualization::ViewItem* diffViewItem,
														 QList<ChangeWithNodes> changesWithNodes)
{
	QString arrowLayer = "move_arrows";
	diffViewItem->setArrowStyle(arrowLayer, "thick");
	for (auto change : changesWithNodes)
	{
		QString highlightOverlayStyle;
		QString highlightOverlayName;

		switch (change.changeType_)
		{
			case FilePersistence::ChangeType::Deletion:
				highlightOverlayName = "delete_highlights";
				highlightOverlayStyle = "delete_no_bg_solid_outline";
				break;
			case FilePersistence::ChangeType::Insertion:
				highlightOverlayName = "insert_highlights";
				highlightOverlayStyle = "insert_no_bg_solid_outline";
				break;
			case FilePersistence::ChangeType::Move:
				highlightOverlayName = "move_highlights";
				highlightOverlayStyle = "move_no_bg_solid_outline";

				// add arrow for the moved node
				diffViewItem->addArrow(const_cast<Model::Node*>(change.oldNode_),
											  const_cast<Model::Node*>(change.newNode_), arrowLayer);
				break;
			case FilePersistence::ChangeType::Stationary:
				highlightOverlayName = "modify_highlights";
				highlightOverlayStyle = "modify_no_bg_solid_outline";
				break;
			case FilePersistence::ChangeType::Unclassified:
				Q_ASSERT(false);
				break;
		}

		QList<Visualization::Item*> items;

		if (change.oldNode_)
			items.append(diffViewItem->findAllVisualizationsOf(change.oldNode_));
		if (change.newNode_)
			items.append(diffViewItem->findAllVisualizationsOf(change.newNode_));

		for (auto item : items)
		{
			auto overlay = new Visualization::HighlightOverlay{item,
					Visualization::HighlightOverlay::itemStyles().get(highlightOverlayStyle)};
			item->addOverlay(overlay, highlightOverlayName);
		}
	}
}

void DiffManager::visualizeChangedNodes(Model::TreeManager* oldVersionManager,
													 QSet<Model::NodeIdType> changedNodesToVisualize,
													 Model::TreeManager* newVersionManager, Visualization::ViewItem* diffViewItem)
{
	for (auto id : changedNodesToVisualize)
	{
		auto oldNode = const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id));

		// old version in left column
		if (!oldNode)
			oldNode = new Model::Text{"node in old version not found"};
		diffViewItem->insertNode(oldNode, 0);

		// new version in right column
		if (!newNode)
			newNode = new Model::Text{"node in new version not found"};

		diffViewItem->insertNode(newNode, 1);
	}
}

Model::TreeManager* DiffManager::createTreeManagerFromVersion(FilePersistence::GitRepository& repository,
																				  QString version)
{
	std::unique_ptr<const FilePersistence::Commit> commit{repository.getCommit(version)};

	auto fileStore = new FilePersistence::SimpleTextFileStore {
				[this, &commit](QString filename, const char*& data, int& size)
				{ return commit->getFileContent(filename, data, size, false); }
			};

	Model::TreeManager* versionTreeManager = new Model::TreeManager{};
	versionTreeManager->load(fileStore, project_, false);

	return versionTreeManager;
}

}