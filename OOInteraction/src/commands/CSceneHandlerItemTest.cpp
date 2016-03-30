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

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/expressions/EmptyExpression.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "FilePersistence/src/version_control/GitRepository.h"

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
		const QStringList&, const std::unique_ptr<Visualization::Cursor>&)
{
	//Test code goes here

	QString projectsDir = "projects/";

	// versions to compare
	QString newCommitId = "HEAD";
	QString oldCommitId = "f1e118bc7e522e2c9404d29d1b21e58956018354";

	// project names
	QString newVersionProject = "MyTest"+newCommitId;
	QString oldVersionProject = "MyTest"+oldCommitId;

	FilePersistence::SimpleTextFileStore store;
	store.setBaseFolder(projectsDir);

	// load newer version
	Model::TreeManager* newVersionManager = new Model::TreeManager{};
	newVersionManager->load(&store, newVersionProject, false);

	// load older version
	Model::TreeManager* oldVersionManager = new Model::TreeManager{};
	oldVersionManager->load(&store, oldVersionProject, false);

	Model::Reference::resolvePending();

	// get GitRepository
	QString path{projectsDir + newVersionProject};
	if (!FilePersistence::GitRepository::repositoryExists(path))
	{
		qDebug() << "no repository found at " + path;
		return new Interaction::CommandResult{};
	}

	FilePersistence::GitRepository repository{path};

	FilePersistence::Diff diff = repository.diff(oldCommitId, newCommitId);
	auto changes = diff.changes();

	QSet<Model::NodeIdType> changedNodesToVisualize;

	auto typeMatcher = Model::SymbolMatcher::guessMatcher("Class");

	for (auto change : changes.values())
	{
		if (change->isFake() || change->onlyStructureChange()) continue;

		auto id = change->nodeId();

		// search one instance of the changed node
		if (auto node = const_cast<Model::Node*>(Model::AllTreeManagers::instance().nodeForId(id)))
		{

			Model::Node* changedNode = nullptr;

			if (auto ancestorNode = node->firstAncestorOfType(typeMatcher))
			{
				changedNode = ancestorNode;
				id = newVersionManager->nodeIdMap().id(changedNode);
			}
			else
				changedNode = node;

			// Ignore empty nodes
			if (auto changedExpr = DCast<OOModel::ExpressionStatement>(changedNode))
				if (DCast<OOModel::EmptyExpression>(changedExpr->expression()))
					continue;

			changedNodesToVisualize.insert(id);
		}
	}

	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(newVersionManager);
	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(oldVersionManager);

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
