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
#include "FilePersistence/src/simple/SimpleTextFileStore.h"
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

	QString testDir = "projects/";
	Model::TreeManager* manager = new Model::TreeManager{};
	FilePersistence::SimpleTextFileStore store;
	store.setBaseFolder(testDir);

	//manager->load(&store, "tetris");
	manager->load(&store, "large", false);
	auto prj = DCast<OOModel::Project> (manager->root());
	Model::Reference::resolvePending();

	Visualization::VisualizationManager::instance().mainScene()->addTopLevelNode(prj);
	Visualization::VisualizationManager::instance().mainScene()->listenToTreeManager(manager);

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
