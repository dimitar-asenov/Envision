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

DiffManager::DiffManager(QString oldVersion, QString newVersion, QString project)
{
	_oldVersion = oldVersion;
	_newVersion = newVersion;
	_project = project;
}

void DiffManager::visualize()
{
	auto diffViewItem = Visualization::VisualizationManager::instance().mainScene()->
			viewItems()->newViewItem("DiffView");

	QString projectsDir = "projects/" + _project;

	// get GitRepository
	if (!FilePersistence::GitRepository::repositoryExists(projectsDir))
	{
		qDebug() << "no repository found at " + projectsDir;
		return;
	}

	FilePersistence::GitRepository repository{projectsDir};

	// load name into tree
	std::unique_ptr<const FilePersistence::Commit> oldCommit{repository.getCommit(_oldVersion)};
	std::unique_ptr<const FilePersistence::Commit> newCommit{repository.getCommit(_newVersion)};

	auto oldFileStore = new FilePersistence::SimpleTextFileStore{
				[this, &oldCommit](QString filename, const char*& data, int& size)
				{ return oldCommit->getFileContent(filename, data, size, false); }
			};
	auto newFileStore = new FilePersistence::SimpleTextFileStore{
			[this, &newCommit](QString filename, const char*& data, int& size)
			{ return newCommit->getFileContent(filename, data, size, false); }
		};

	// load newer version
	Model::TreeManager* newVersionManager = new Model::TreeManager{};
	newVersionManager->load(newFileStore, _project, false);

	// load older version
	Model::TreeManager* oldVersionManager = new Model::TreeManager{};
	oldVersionManager->load(oldFileStore, _project, false);

	Model::Reference::resolvePending();

	FilePersistence::Diff diff = repository.diff(_oldVersion, _newVersion);

	auto changes = diff.changes();

	const int INDEX_OF_DELETE = 0;
	const int INDEX_OF_ADD = 1;
	const int INDEX_OF_MOVE = 2;
	const int INDEX_OF_MODIFY = 3;

	QList<QSet<Model::NodeIdType>> nodeLists;
	for (int i = 0; i < 4; i++)
		nodeLists.append(QSet<Model::NodeIdType>{});

	QSet<Model::NodeIdType> changedNodesToVisualize;

	for (auto change : changes.values())
	{
		if (change->isFake() || change->onlyStructureChange()) continue;

		auto id = change->nodeId();

		switch (change->type())
		{
			case FilePersistence::ChangeType::Deletion:
				nodeLists[INDEX_OF_DELETE].insert(id);
				break;
			case FilePersistence::ChangeType::Insertion:
				nodeLists[INDEX_OF_ADD].insert(id);
				break;
			case FilePersistence::ChangeType::Move:
				nodeLists[INDEX_OF_MOVE].insert(id);
				break;
			case FilePersistence::ChangeType::Stationary:
				nodeLists[INDEX_OF_MODIFY].insert(id);
				break;
			case FilePersistence::ChangeType::Unclassified:
				qDebug() << "Unclassified";
				break;
		}
		Model::NodeIdType nodeId;
		if (change->type() != FilePersistence::ChangeType::Deletion)
			if (findChangedNode(newVersionManager, id, nodeId))
				changedNodesToVisualize.insert(nodeId);
		if (change->type() != FilePersistence::ChangeType::Insertion)
			if (findChangedNode(oldVersionManager, id, nodeId))
				changedNodesToVisualize.insert(nodeId);
	}
	//
	for (int i = 0; i < nodeLists.size(); i++)
		removeDirectChildrenOfNodesInContainer(&nodeLists[i]);

	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(newVersionManager);
	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(oldVersionManager);

	diffViewItem->setMajorAxis(Visualization::GridLayouter::NoMajor);

	// add nodes to visualization, if not avaible show text
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

	//TODO method for visualizations

	// create highlights and arrows for moved nodes
	for (auto id : nodeLists[INDEX_OF_MOVE])
	{
		auto oldNode = const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id));

		diffViewItem->addArrow(oldNode, newNode, QString{"move_arrows"});

		Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
							  [oldNode, newNode, diffViewItem](){
			auto items = diffViewItem->findAllVisualizationsOf(oldNode);
			items.append(diffViewItem->findAllVisualizationsOf(newNode));
			for (auto item : items)
			{
				auto overlay = new Visualization::HighlightOverlay{item,
						Visualization::HighlightOverlay::itemStyles().get("move_no_bg_solid")};
				item->addOverlay(overlay, "move_highlights");
			}});
	}

	// create highlights for deleted nodes
	for (auto id : nodeLists[INDEX_OF_DELETE])
	{
		auto oldNode = const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id));

		if (!oldNode)
			continue;

		 Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								[oldNode, diffViewItem](){
			 auto oldItems =  diffViewItem->findAllVisualizationsOf(oldNode);
			 for (auto item : oldItems)
			 {
				 auto overlay = new Visualization::HighlightOverlay{item,
						 Visualization::HighlightOverlay::itemStyles().get("delete_no_bg_solid")};
				 item->addOverlay(overlay, "delete_highlights");
			 }});

	}

	// create highlights for added nodes
	for (auto id : nodeLists[INDEX_OF_ADD])
	{
		auto newNode = const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id));

		Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
							  [newNode, diffViewItem](){
			auto newItems =  diffViewItem->findAllVisualizationsOf(newNode);
			for (auto item : newItems)
			{
				auto overlay = new Visualization::HighlightOverlay{item,
						Visualization::HighlightOverlay::itemStyles().get("insert_no_bg_solid")};
				item->addOverlay(overlay, "insert_highlights");
			}});
	}

	// create highlights for modified nodes
	for (auto id : nodeLists[INDEX_OF_MODIFY])
	{
		auto oldNode = const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id));

		Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
							  [oldNode, newNode, diffViewItem](){
			auto items = diffViewItem->findAllVisualizationsOf(oldNode);
			items.append(diffViewItem->findAllVisualizationsOf(newNode));
			for (auto item : items)
			{
				auto overlay = new Visualization::HighlightOverlay{item,
						Visualization::HighlightOverlay::itemStyles().get("modify_no_bg_solid")};
				item->addOverlay(overlay, "modify_highlights");
			}});
	}

	// switch to the newly created view
	Visualization::VisualizationManager::instance().mainScene()->viewItems()->switchToView(diffViewItem);
}

/**
 * Deletes all nodes in \a container if their direct parent is also in \container.
 */
template<typename Container>
void DiffManager::removeDirectChildrenOfNodesInContainer(Container* container)
{
	auto it = container->begin();
	while (it != container->end())
	{
		auto nodeInContainer = Model::AllTreeManagers::instance().nodeForId(*it);
		auto parentNode = nodeInContainer->parent();

		if (container->contains(Model::AllTreeManagers::instance().idForNode(parentNode)))
			it = container->erase(it);
		else
			it++;
	}
}

// TODO change method to use type<> to specify ancestor
/**
 * Searches in \a treeManager for a node with \a id and tries to get it's first ancestor of type OOModel::Class.
 * Returns true if such a node was found and saved to \a resultId.
 */
bool DiffManager::findChangedNode(Model::TreeManager* treeManager, Model::NodeIdType id, Model::NodeIdType& resultId)
{
	if (auto node = const_cast<Model::Node*>(treeManager->nodeIdMap().node(id)))
	{

		Model::Node* changedNode = nullptr;

		// TODO fixed type, maybe more dynamic?
		if (auto ancestorNode = node->firstAncestorOfType<OOModel::Class>())
		{
			changedNode = ancestorNode;

			// Ignore empty nodes
			if (auto changedExpr = DCast<OOModel::ExpressionStatement>(changedNode))
				if (DCast<OOModel::EmptyExpression>(changedExpr->expression()))
					return false;

			resultId = treeManager->nodeIdMap().id(changedNode);
			return true;

		}
	}
	return false;
}

}
