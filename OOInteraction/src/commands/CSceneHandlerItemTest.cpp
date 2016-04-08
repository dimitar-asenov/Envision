/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "CSceneHandlerItemTest.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/nodes/ViewItemNode.h"
#include "VisualizationBase/src/declarative/GridLayouter.h"
#include "VisualizationBase/src/overlays/HighlightOverlay.h"

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/expressions/EmptyExpression.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "FilePersistence/src/version_control/GitRepository.h"
#include "FilePersistence/src/version_control/History.h"

#include "ModelBase/src/model/AllTreeManagers.h"
#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Reference.h"


namespace OOInteraction {

CSceneHandlerItemTest::CSceneHandlerItemTest() : Command{"test"}{}

bool CSceneHandlerItemTest::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens,
		const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() == 1 && commandTokens.first() == "test";
}

Interaction::CommandResult* CSceneHandlerItemTest::execute(Visualization::Item*, Visualization::Item*,
		const QStringList&, const std::unique_ptr<Visualization::Cursor>&) __attribute__ ((optnone))
{
	//Test code goes here

	QString projectName = "DiffTest";
	QString projectsDir = "projects/" + projectName;

	// get GitRepository
	if (!FilePersistence::GitRepository::repositoryExists(projectsDir))
	{
		qDebug() << "no repository found at " + projectsDir;
		return new Interaction::CommandResult{};
	}

	FilePersistence::GitRepository repository{projectsDir};

	// versions to compare
	QString newCommitId = "HEAD";
	QString oldCommitId = "HEAD^^^^";

	// load name into tree
	std::unique_ptr<const FilePersistence::Commit> oldCommit{repository.getCommit(oldCommitId)};
	std::unique_ptr<const FilePersistence::Commit> newCommit{repository.getCommit(newCommitId)};

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
	newVersionManager->load(newFileStore, projectName, false);

	// load older version
	Model::TreeManager* oldVersionManager = new Model::TreeManager{};
	oldVersionManager->load(oldFileStore, projectName, false);

	Model::Reference::resolvePending();

	FilePersistence::Diff diff = repository.diff(oldCommitId, newCommitId);

	auto changes = diff.changes();

	QSet<Model::NodeIdType> changedNodesToVisualize;
	QSet<Model::NodeIdType> movedNodes;
	QSet<Model::NodeIdType> deletedNodes;
	QSet<Model::NodeIdType> insertedNodes;
	QSet<Model::NodeIdType> modifiedNodes;

	for (auto change : changes.values())
	{
		if (change->isFake() || change->onlyStructureChange()) continue;

		auto id = change->nodeId();

		switch (change->type())
		{
			case FilePersistence::ChangeType::Deletion:
				qDebug() << "Deletion";
				deletedNodes.insert(id);
				break;
			case FilePersistence::ChangeType::Insertion:
				qDebug() << "Insertion";
				insertedNodes.insert(id);
				break;
			case FilePersistence::ChangeType::Move:
				qDebug() << "Move";
				movedNodes.insert(id);
				break;
			case FilePersistence::ChangeType::Stationary:
				qDebug() << "Stationary";
				modifiedNodes.insert(id);
				break;
			case FilePersistence::ChangeType::Unclassified:
				qDebug() << "Unclassified";
				break;
		}


		// search one instance of the changed node NEW VERSION MANAGER
		if (auto node = const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id)))
		{

			Model::Node* changedNode = nullptr;

			if (auto ancestorNode = node->firstAncestorOfType<OOModel::Class>())
			{
				changedNode = ancestorNode;

				// Ignore empty nodes
				if (auto changedExpr = DCast<OOModel::ExpressionStatement>(changedNode))
					if (DCast<OOModel::EmptyExpression>(changedExpr->expression()))
						continue;

				if (change->type() != FilePersistence::ChangeType::Deletion)
				{
					auto newId = newVersionManager->nodeIdMap().id(changedNode);
					changedNodesToVisualize.insert(newId);
				}
			}
		}

		// search one instance of the changed node OLD VERSION MANAGER
		if (auto node = const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id)))
		{

			Model::Node* changedNode = nullptr;

			if (auto ancestorNode = node->firstAncestorOfType<OOModel::Class>())
			{
				changedNode = ancestorNode;

				// Ignore empty nodes
				if (auto changedExpr = DCast<OOModel::ExpressionStatement>(changedNode))
					if (DCast<OOModel::EmptyExpression>(changedExpr->expression()))
						continue;

				if (change->type() != FilePersistence::ChangeType::Insertion)
				{
					auto newId = oldVersionManager->nodeIdMap().id(changedNode);
					changedNodesToVisualize.insert(newId);
				}
			}
		}
	}

	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(newVersionManager);
	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(oldVersionManager);

	Visualization::VisualizationManager::instance().mainScene()->currentViewItem()
			->setMajorAxis(Visualization::GridLayouter::NoMajor);

	for (auto id : changedNodesToVisualize)
	{
		auto oldNode = const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id));


		if (!oldNode)
			oldNode = new Model::Text{"node in old version not found"};

		Visualization::VisualizationManager::instance().mainScene()->addTopLevelNode(oldNode, 0);

		if (!newNode)
			newNode = new Model::Text{"node in new version not found"};

		Visualization::VisualizationManager::instance().mainScene()->addTopLevelNode(newNode, 1);
	}

	for (auto id : movedNodes)
	{
		auto oldNode = const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id));

		Visualization::VisualizationManager::instance().mainScene()->currentViewItem()
				->addArrow(oldNode, newNode, QString{"move_arrows"});

		Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
							  [oldNode, newNode](){
			auto items = Visualization::VisualizationManager::instance().mainScene()->currentViewItem()
							->findAllVisualizationsOf(oldNode);
			items.append(Visualization::VisualizationManager::instance().mainScene()->currentViewItem()
							 ->findAllVisualizationsOf(newNode));
			for (auto item : items)
			{
				auto overlay = new Visualization::HighlightOverlay{item};
				QColor blue{"blue"};
				blue.setAlpha(90);
				overlay->setColor(blue);
				item->addOverlay(overlay, "moved_highlights");
			}});
	}

	for (auto id : deletedNodes)
	{
		auto oldNode = const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id));

		if (!oldNode)
			continue;

		 Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
								[oldNode](){
			 auto oldItems =  Visualization::VisualizationManager::instance().mainScene()->currentViewItem()
							 ->findAllVisualizationsOf(oldNode);
			 for (auto item : oldItems)
			 {
				 auto overlay = new Visualization::HighlightOverlay{item};
				 QColor red{"red"};
				 red.setAlpha(90);
				 overlay->setColor(red);
				 item->addOverlay(overlay, "deleted_highlights");
			 }});

	}

	for (auto id: insertedNodes)
	{
		auto newNode = const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id));

		Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
							  [newNode](){
			auto newItems =  Visualization::VisualizationManager::instance().mainScene()->currentViewItem()
							->findAllVisualizationsOf(newNode);
			for (auto item : newItems)
			{
				auto overlay = new Visualization::HighlightOverlay{item};
				QColor green{"green"};
				green.setAlpha(90);
				overlay->setColor(green);
				item->addOverlay(overlay, "inserted_highlights");
			}});
	}

	for (auto id : modifiedNodes)
	{
		auto oldNode = const_cast<Model::Node*>(oldVersionManager->nodeIdMap().node(id));
		auto newNode = const_cast<Model::Node*>(newVersionManager->nodeIdMap().node(id));

		Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
							  [oldNode, newNode](){
			auto items = Visualization::VisualizationManager::instance().mainScene()->currentViewItem()
							->findAllVisualizationsOf(oldNode);
			items.append(Visualization::VisualizationManager::instance().mainScene()->currentViewItem()
							 ->findAllVisualizationsOf(newNode));
			for (auto item : items)
			{
				auto overlay = new Visualization::HighlightOverlay{item};
				QColor yellow{"yellow"};
				yellow.setAlpha(90);
				overlay->setColor(yellow);
				item->addOverlay(overlay, "modified_highlights");
			}});
	}


	return new Interaction::CommandResult{};
}

QList<Interaction::CommandSuggestion*> CSceneHandlerItemTest::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	QList<Interaction::CommandSuggestion*> s;
	if (QString{"test"}.startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
			s.append(new Interaction::CommandSuggestion{"test", "Loads and visualizes a large project"});
	return s;
}

}
