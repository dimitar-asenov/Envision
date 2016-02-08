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
#include "VisualizationBase/src/overlays/SelectionOverlay.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"
#include "VisualizationBase/src/CustomSceneEvent.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "FilePersistence/src/version_control/GitRepository.h"

using namespace Visualization;
using namespace FilePersistence;

namespace Interaction {

static const QString overlayGroupName{"DiffHighlights"};

CDiff::CDiff() : CommandWithFlags{"diff", {{"project"}}, true, false}
{}

CommandResult* CDiff::executeNamed(Visualization::Item* /*source*/, Visualization::Item* target,
											  const std::unique_ptr<Visualization::Cursor>& /*cursor*/,
											  const QString& name, const QStringList& /*attributes*/)
{
	auto scene = target->scene();
	scene->clearFocus();
	scene->clearSelection();
	scene->setMainCursor(nullptr);

	Model::TreeManager* headManager = target->node()->manager();
	QString managerName = headManager->name();

	// get GitRepository
	QString path{"projects/"};
	path.append(managerName);
	std::shared_ptr<FilePersistence::GitRepository> repository(new GitRepository{path});

	headManager->setName("HEAD");

	// load name into tree
	std::unique_ptr<const Commit> commit{repository->getCommit(name)};

	auto fileStore = new SimpleTextFileStore{
				[this, &commit](QString filename, const char*& data, int& size)
				{ return commit->getFileContent(filename, data, size); }
			};

	auto revisionManager = new Model::TreeManager{};
	revisionManager->load(fileStore, managerName, false);
	revisionManager->setName(name);

	headManager->setName(managerName);

	// build visualization
	Item* headRoot = target;
	while (headRoot->parent()) headRoot = headRoot->parent();

	Item* revisionRoot = new RootItem{revisionManager->root()};
	revisionRoot->setPos(-400.f, 0.f);

	VisualizationManager::instance().mainScene()->addTopLevelItem(revisionRoot);
	VisualizationManager::instance().mainScene()->listenToTreeManager(revisionManager);

	QApplication::postEvent(Visualization::VisualizationManager::instance().mainScene(),
		new Visualization::CustomSceneEvent{[headRoot, headManager, revisionRoot, revisionManager,
														name, target, repository]()
	{
			Diff diff = repository->diff(name, GitRepository::WORKDIR);
			IdToChangeDescriptionHash changes = diff.changes();

			auto scene = VisualizationManager::instance().mainScene();
			auto overlayGroup = scene->overlayGroup(overlayGroupName);
			if (!overlayGroup) overlayGroup = scene->addOverlayGroup(overlayGroupName);

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
			Model::Node* parent = nullptr;
			Model::NodeIdType parentID;
			for (auto id : relevantIDs)
			{
				iter = changes.find(id);
				if (iter != changes.end())
				{
					auto change = iter.value();
					switch (change->type())
					{
						case ChangeType::Insertion:
							node = const_cast<Model::Node*>(headManager->nodeIdMap().node(id));
							parent = node->parent();
							if (auto item = headRoot->findVisualizationOf(node))
							{
								if (parent)
								{
									parentID = headManager->nodeIdMap().id(parent);
									iter = changes.find(parentID);
									if (iter != changes.end())
									{
										auto parentChange = iter.value();
										if (parentChange->type() != ChangeType::Insertion)
											overlayGroup->addOverlay(makeOverlay(new Visualization::SelectionOverlay{item,
												Visualization::SelectionOverlay::itemStyles().get("insert")}));
									}
									else
										overlayGroup->addOverlay(makeOverlay(new Visualization::SelectionOverlay{item,
											Visualization::SelectionOverlay::itemStyles().get("insert")}));
								}
								else
									overlayGroup->addOverlay(makeOverlay(new Visualization::SelectionOverlay{item,
										Visualization::SelectionOverlay::itemStyles().get("insert")}));
							}
							break;

						case ChangeType::Deletion:
							node = const_cast<Model::Node*>(revisionManager->nodeIdMap().node(id));
							parent = node->parent();
							if (auto item = revisionRoot->findVisualizationOf(node))
							{
								if (parent)
								{
									parentID = revisionManager->nodeIdMap().id(parent);
									iter = changes.find(parentID);
									if (iter != changes.end())
									{
										auto parentChange = iter.value();
										if (parentChange->type() != ChangeType::Deletion)
											overlayGroup->addOverlay(makeOverlay(new Visualization::SelectionOverlay{item,
												Visualization::SelectionOverlay::itemStyles().get("delete")}));
									}
									else
										overlayGroup->addOverlay(makeOverlay(new Visualization::SelectionOverlay{item,
											Visualization::SelectionOverlay::itemStyles().get("delete")}));
								}
								else
									overlayGroup->addOverlay(makeOverlay(new Visualization::SelectionOverlay{item,
										Visualization::SelectionOverlay::itemStyles().get("delete")}));
							}
							break;

						case ChangeType::Move:
							node = const_cast<Model::Node*>(headManager->nodeIdMap().node(id));
							if (auto item = headRoot->findVisualizationOf(node))
								overlayGroup->addOverlay(makeOverlay(new Visualization::SelectionOverlay{item,
									Visualization::SelectionOverlay::itemStyles().get("move")}));

							node = const_cast<Model::Node*>(revisionManager->nodeIdMap().node(id));
							if (auto item = revisionRoot->findVisualizationOf(node))
								overlayGroup->addOverlay(makeOverlay(new Visualization::SelectionOverlay{item,
									Visualization::SelectionOverlay::itemStyles().get("move")}));
							break;

						default:
							break;
					} // end switch

					// test update flag
					node = const_cast<Model::Node*>(headManager->nodeIdMap().node(id));
					if (auto item = headRoot->findVisualizationOf(node))
						if (change->flags().testFlag(ChangeDescription::UpdateType::Value))
							overlayGroup->addOverlay(makeOverlay(new Visualization::SelectionOverlay{item,
								Visualization::SelectionOverlay::itemStyles().get("update")}));


				}
				else
					Q_ASSERT(false);
			}


	} } );

	return new CommandResult{};
}

QStringList CDiff::possibleNames(Visualization::Item* /*source*/, Visualization::Item* target,
											const std::unique_ptr<Visualization::Cursor>& /*cursor*/)
{
	Model::TreeManager* headManager = target->node()->manager();
	QString managerName = headManager->name();

	// get GitRepository
	QString path{"projects/"};
	path.append(managerName);

	QStringList names;
	if (GitRepository::repositoryExists(path))
	{
		GitRepository repository{path};

		names.append(repository.localBranches());
		names.append(repository.tags());
		names.append(repository.revisions());
	}
	return names;
}

}
