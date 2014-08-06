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

#include "CDiffTest.h"

#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "ModelBase/src/model/TreeManager.h"

#include "FilePersistence/src/version_control/ChangeDescription.h"
#include "FilePersistence/src/version_control/Commit.h"
#include "VisualizationBase/src/items/Item.h"

#include "VisualizationBase/src/CustomSceneEvent.h"

using namespace Visualization;

namespace Interaction {

CDiffTest::CDiffTest() : CommandWithNameAndFlags{"diffTest", {}, true}
{
	QString path("projects/Hello/.git");
	repository_ = new FilePersistence::GitRepository(path);
}

CommandResult* CDiffTest::executeNamed(Visualization::Item* source, Visualization::Item* target,
		const std::unique_ptr<Visualization::Cursor>& cursor,
		const QString& name, const QStringList& attributes)
{
	(void) source;
	(void) target;
	(void) cursor;
	(void) attributes;

	QString oldRevision("abf18");
	const FilePersistence::Commit* oldCommit = repository_->getCommit(oldRevision);

	auto oldFileStore = new FilePersistence::SimpleTextFileStore(
				[this, &oldCommit](QString name, const char*& data, int& size)
				{ return oldCommit->getFileContent(name, data, size); }
			);

	auto managerOld = new Model::TreeManager();
	managerOld->load(oldFileStore, name, false);
	managerOld->setName("Hello (Old)");

	QString newRevision("413d2");
	const FilePersistence::Commit* newCommit = repository_->getCommit(newRevision);

	auto newFileStore = new FilePersistence::SimpleTextFileStore(
				[this, &newCommit](QString name, const char*& data, int& size)
				{ return newCommit->getFileContent(name, data, size); }
			);

	auto managerNew = new Model::TreeManager();
	managerNew->load(newFileStore, name, false);
	managerNew->setName("Hello (New)");

	Item* oldRoot = new RootItem(managerOld->root());
	oldRoot->setPos(-200.f, 0.f);

	Item* newRoot = new RootItem(managerNew->root());
	newRoot->setPos(200.f, 0.f);

	VisualizationManager::instance().mainScene()->addTopLevelItem(oldRoot);
	VisualizationManager::instance().mainScene()->listenToTreeManager(managerOld);

	VisualizationManager::instance().mainScene()->addTopLevelItem(newRoot);
	VisualizationManager::instance().mainScene()->listenToTreeManager(managerNew);

	QApplication::postEvent(Visualization::VisualizationManager::instance().mainScene(),
		new Visualization::CustomSceneEvent( [oldRoot, newRoot, managerOld, managerNew, oldRevision, newRevision, this](){

			FilePersistence::Diff diff = repository_->diff(oldRevision, newRevision);
			FilePersistence::IdToChangeDescriptionHash changes = diff.changes();

			auto insertHighlight = VisualizationManager::instance().mainScene()->addHighlight("Insert", "green");
			auto deleteHighlight = VisualizationManager::instance().mainScene()->addHighlight("Delete", "red");

			Model::Node* node = nullptr;
			for (FilePersistence::ChangeDescription* change : changes.values())
			{
				switch (change->type())
				{
					case FilePersistence::ChangeType::Added:
						node = const_cast<Model::Node*>(managerNew->nodeIdMap().node(change->id()));
						if (auto item = newRoot->findVisualizationOf(node))
							insertHighlight->addHighlightedItem(item);
						break;

					case FilePersistence::ChangeType::Deleted:
						node = const_cast<Model::Node*>(managerOld->nodeIdMap().node(change->id()));
						if (auto item = oldRoot->findVisualizationOf(node))
							deleteHighlight->addHighlightedItem(item);
						break;

					default:
						break;
				}
			}

	} ) );

	delete oldCommit;
	delete newCommit;

	return new CommandResult();
}

QStringList CDiffTest::availableProjectsOnDisk()
{
	auto dir = QDir( "projects/" );
	return dir.entryList( QDir::AllDirs | QDir::NoDot | QDir::NoDotDot, QDir::Name);
}

QStringList CDiffTest::possibleNames()
{
	return availableProjectsOnDisk();
}

} /* namespace Interaction */
