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
using namespace FilePersistence;

namespace Interaction {

CDiff::CDiff() : CommandWithNameAndFlags{"diff", {{"project"}}, false}
{}

CommandResult* CDiff::executeNamed(Visualization::Item*, Visualization::Item* target,
											  const std::unique_ptr<Visualization::Cursor>&,
											  const QString& name, const QStringList&)
{
	Model::TreeManager* headManager = target->node()->manager();
	QString managerName = headManager->name();

	// get GitRepository
	QString path("projects/");
	path.append(managerName);
	repository = new GitRepository(path);

	headManager->setName("HEAD");

	// load name into tree
	const Commit* commit = repository->getCommit(name);

	auto fileStore = new SimpleTextFileStore(
				[this, &commit](QString filename, const char*& data, int& size)
				{ return commit->getFileContent(filename, data, size); }
			);

	auto revisionManager = new Model::TreeManager();
	revisionManager->load(fileStore, managerName, false);
	revisionManager->setName(name);

	// build visualization
	Item* headRoot = target;
	while (headRoot->parent()) headRoot = headRoot->parent();

	Item* revisionRoot = new RootItem(revisionManager->root());
	revisionRoot->setPos(-400.f, 0.f);

	VisualizationManager::instance().mainScene()->addTopLevelItem(revisionRoot);
	VisualizationManager::instance().mainScene()->listenToTreeManager(revisionManager);

	QApplication::postEvent(Visualization::VisualizationManager::instance().mainScene(),
		new Visualization::CustomSceneEvent([headRoot, headManager, revisionRoot, revisionManager,
														name, target, this]()
	{
			Diff diff = repository->diff(name, GitRepository::WORKDIR);
			IdToChangeDescriptionHash changes = diff.changes();

			auto insertHighlight = VisualizationManager::instance().mainScene()->addHighlight("Insert", "green");
			auto deleteHighlight = VisualizationManager::instance().mainScene()->addHighlight("Delete", "red");

			IdToChangeDescriptionHash::iterator iter;

			QSet<Model::NodeIdType> relevantIDs;

			QSet<Model::NodeIdType> completed;
			QList<Model::Node*> stack;
			stack.append(target->node());
			while (!stack.isEmpty())
			{
				// HEAD version
				Model::Node* current = stack.takeLast();

				stack.append(current->children());

				// check if relevant
				Model::NodeIdType currentID = headManager->nodeIdMap().id(current);
				//qDebug() << currentID.toString();
				iter = changes.find(currentID);
				if (iter != changes.end())
					relevantIDs.insert(currentID);

				// REVISION version
				if (!completed.contains(currentID))
				{
					QList<Model::Node*> revisionStack;
					Model::Node* revisionNode = const_cast<Model::Node*>(revisionManager->nodeIdMap().node(currentID));
					if (revisionNode != nullptr)
					{
						revisionStack.append(revisionNode);
						while (!revisionStack.isEmpty())
						{
							revisionNode = revisionStack.takeLast();
							Model::NodeIdType revisionNodeID = revisionManager->nodeIdMap().id(revisionNode);
							completed.insert(revisionNodeID);

							revisionStack.append(revisionNode->children());

							// check if relevant
							iter = changes.find(revisionNodeID);
							if (iter != changes.end())
								relevantIDs.insert(revisionNodeID);
						}
					}
				}
			}

			Model::Node* node = nullptr;
			for (auto id : relevantIDs)
			{
				iter = changes.find(id);
				if (iter != changes.end())
				{
					ChangeDescription* change = iter.value();
					switch (change->type())
					{
						case ChangeType::Added:
							node = const_cast<Model::Node*>(headManager->nodeIdMap().node(id));
							if (auto item = headRoot->findVisualizationOf(node))
								insertHighlight->addHighlightedItem(item);
							break;

						case ChangeType::Deleted:
							node = const_cast<Model::Node*>(revisionManager->nodeIdMap().node(id));
							if (auto item = revisionRoot->findVisualizationOf(node))
								deleteHighlight->addHighlightedItem(item);
							break;

						default:
							break;
					}
				}
				else
					Q_ASSERT(false);
			}


	} ) );

	SAFE_DELETE(commit);

	return new CommandResult();
}

QStringList CDiff::possibleNames()
{
	return QStringList();
}

} /* namespace Interaction */
