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

#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "ModelBase/src/model/TreeManager.h"

#include "FilePersistence/src/version_control/ChangeDescription.h"
#include "VisualizationBase/src/items/Item.h"

using namespace Visualization;

namespace Interaction {

CDiff::CDiff() : CommandWithNameAndFlags{"diff", {{"library"}}, true}
{
	QString path("/home/motth/Documents/Envision/Envision/DebugBuild/projects/Hello/.git");
	repository_ = new FilePersistence::GitRepository(path);
}

CommandResult* CDiff::executeNamed(Visualization::Item*, Visualization::Item*,
			const QString& name, const QStringList& attributes)
{
	QString commitOld("abf18");
	repository_->checkout(commitOld, true);

	auto managerOld = new Model::TreeManager();
	managerOld->load(new FilePersistence::SimpleTextFileStore("projects/"), name, attributes.first() == "library");
	managerOld->setName("Hello (Old)");

	QString commitNew("413d2");
	repository_->checkout(commitNew, true);

	auto managerNew = new Model::TreeManager();
	managerNew->load(new FilePersistence::SimpleTextFileStore("projects/"), name, attributes.first() == "library");
	managerNew->setName("Hello (New)");

	FilePersistence::Diff diff = repository_->diff(commitOld, commitNew);
	FilePersistence::IdToChangeDescriptionHash changes = diff.changes();

	Item* oldRoot = new RootItem(managerOld->root());
	oldRoot->setPos(-200.f, 0.f);

	Item* newRoot = new RootItem(managerNew->root());
	newRoot->setPos(200.f, 0.f);

	QList<Visualization::Item*> stack;

	for (FilePersistence::ChangeDescription* change : changes.values())
	{
		Model::Node* node = const_cast<Model::Node*>(Model::NodeIdMap::node(change->id()));
		if (node->parent() == nullptr)
			qDebug() << "nullptr";
		Item* item = nullptr;
		// FIXME: No visualizations are found
		item = newRoot->findVisualizationOf(node);
		if (item != nullptr)
		{
			qDebug() << "Test";
			stack.append(item);
		}
	}

	VisualizationManager::instance().mainScene()->addTopLevelItem(oldRoot);
	VisualizationManager::instance().mainScene()->listenToTreeManager(managerOld);

	VisualizationManager::instance().mainScene()->addTopLevelItem(newRoot);
	VisualizationManager::instance().mainScene()->listenToTreeManager(managerNew);

	return new CommandResult();
}

QStringList CDiff::availableProjectsOnDisk()
{
	auto dir = QDir( "projects/" );
	return dir.entryList( QDir::AllDirs | QDir::NoDot | QDir::NoDotDot, QDir::Name);
}

QStringList CDiff::possibleNames()
{
	return availableProjectsOnDisk();
}

} /* namespace Interaction */
