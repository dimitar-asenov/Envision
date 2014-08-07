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

#include "CDiff.h"

#include "ModelBase/src/model/TreeManager.h"

#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/CustomSceneEvent.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"

using namespace Visualization;

namespace Interaction {

CDiff::CDiff() : CommandWithNameAndFlags{"diff", {{"project"}}, false}
{}

CommandResult* CDiff::executeNamed(Visualization::Item*, Visualization::Item* target,
											  const std::unique_ptr<Visualization::Cursor>&,
											  const QString& name, const QStringList& attributes)
{
	// TODO only display affected subtree
	(void) attributes;

	Model::TreeManager* manager = target->node()->manager();
	QString managerName = manager->name();

	// get GitRepository
	QString path("projects/");
	path.append(managerName);
	repository = new FilePersistence::GitRepository(path);

	// load HEAD into tree
	auto headManager = new Model::TreeManager();
	headManager->load(new FilePersistence::SimpleTextFileStore("projects/"), managerName, false);
	headManager->setName("HEAD");

	// load name into tree
	const FilePersistence::Commit* commit = repository->getCommit(name);

	auto fileStore = new FilePersistence::SimpleTextFileStore(
				[this, &commit](QString filename, const char*& data, int& size)
				{ return commit->getFileContent(filename, data, size); }
			);

	auto revisionManager = new Model::TreeManager();
	revisionManager->load(fileStore, managerName, false);
	revisionManager->setName(name);

	// build visualization
	Item* headRoot = new RootItem(headManager->root());

	Item* revisionRoot = new RootItem(revisionManager->root());
	revisionRoot->setPos(-400.f, 0.f);

	VisualizationManager::instance().mainScene()->addTopLevelItem(headRoot);
	VisualizationManager::instance().mainScene()->listenToTreeManager(headManager);

	VisualizationManager::instance().mainScene()->addTopLevelItem(revisionRoot);
	VisualizationManager::instance().mainScene()->listenToTreeManager(revisionManager);

	QApplication::postEvent(Visualization::VisualizationManager::instance().mainScene(),
		new Visualization::CustomSceneEvent([headRoot, headManager, revisionRoot, revisionManager,
														name, this]()
	{
			FilePersistence::Diff diff = repository->diff(name, FilePersistence::GitRepository::WORKDIR);
			FilePersistence::IdToChangeDescriptionHash changes = diff.changes();

			auto insertHighlight = VisualizationManager::instance().mainScene()->addHighlight("Insert", "green");
			auto deleteHighlight = VisualizationManager::instance().mainScene()->addHighlight("Delete", "red");

			Model::Node* node = nullptr;
			for (FilePersistence::ChangeDescription* change : changes.values())
			{
				switch (change->type())
				{
					case FilePersistence::ChangeType::Added:
						node = const_cast<Model::Node*>(headManager->nodeIdMap().node(change->id()));
						if (auto item = headRoot->findVisualizationOf(node))
							insertHighlight->addHighlightedItem(item);
						break;

					case FilePersistence::ChangeType::Deleted:
						node = const_cast<Model::Node*>(revisionManager->nodeIdMap().node(change->id()));
						if (auto item = revisionRoot->findVisualizationOf(node))
							deleteHighlight->addHighlightedItem(item);
						break;

					default:
						break;
				}
			}

	} ) );

	delete commit;

	return new CommandResult();
}

QStringList CDiff::possibleNames()
{
	return QStringList();
}

} /* namespace Interaction */
