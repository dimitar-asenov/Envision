/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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
***********************************************************************************************************************/

#include "CMerge.h"

#include "ModelBase/src/model/TreeManager.h"

#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/CustomSceneEvent.h"

#include "FilePersistence/src/version_control/GitRepository.h"
#include "FilePersistence/src/simple/SimpleTextFileStore.h"

using namespace Visualization;
using namespace FilePersistence;

namespace Interaction {

CMerge::CMerge() : CommandWithFlags{"merge", {{"project"}, {"NoFastForward"}}, false}
{}

CommandResult* CMerge::executeNamed(Visualization::Item* /*source*/, Visualization::Item* target,
												const std::unique_ptr<Visualization::Cursor>& /*cursor*/,
												const QString& name, const QStringList& /*attributes*/)
{
	auto scene = target->scene();
	scene->clearFocus();
	scene->setMainCursor(nullptr);

	Model::TreeManager* headManager = target->node()->manager();
	QString managerName = headManager->name();

	// get GitRepository
	QString path{"projects/"};
	path.append(managerName);
	std::shared_ptr<GitRepository> repository{new GitRepository{path}};

	bool useFastForward = false;

	std::shared_ptr<Merge> merge = repository->merge(name, useFastForward);

	// TODO: handle different merge scenarios!
	if (merge->mergedTree())
	{
		// load name into tree
		const Commit* commit = repository->getCommit(name);

		// TODO check which behavior is correct for fileName matching
		auto fileStoreRevision = new SimpleTextFileStore{
					[this, &commit](QString filename, const char*& data, int& size)
					{ return commit->getFileContent(filename, data, size, true); }
				};

		auto revisionManager = new Model::TreeManager{};
		revisionManager->load(fileStoreRevision, managerName, false);
		revisionManager->setName(name);
		headManager->setName(managerName);


		auto mergeManager = new Model::TreeManager{};
		auto fileStoreMerge = new SimpleTextFileStore{merge->mergedTree().get()};
		mergeManager->load(fileStoreMerge, managerName, false);
		mergeManager->setName("Merge");


		Item* mergeRoot = new RootItem{mergeManager->root()};
		mergeRoot->setPos(400.f, 0.f);

		Item* revisionRoot = new RootItem{revisionManager->root()};
		revisionRoot->setPos(800.f, 0.f);

		VisualizationManager::instance().mainScene()->addTopLevelItem(mergeRoot);
		VisualizationManager::instance().mainScene()->listenToTreeManager(mergeManager);

		VisualizationManager::instance().mainScene()->addTopLevelItem(revisionRoot);
		VisualizationManager::instance().mainScene()->listenToTreeManager(revisionManager);
	}
	else
		Q_ASSERT(false);

	return new CommandResult{};
}

QStringList CMerge::possibleNames(Visualization::Item* /*source*/, Visualization::Item* /*target*/,
											 const std::unique_ptr<Visualization::Cursor>& /*cursor*/)
{
	return QStringList{};
}

}
