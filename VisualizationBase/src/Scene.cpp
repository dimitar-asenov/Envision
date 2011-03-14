/***********************************************************************************************************************
 * Scene.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Scene.h"
#include "VisualizationManager.h"
#include "items/Item.h"
#include "items/SceneHandlerItem.h"
#include "items/SelectedItem.h"

#include "ModelBase/headers/nodes/Node.h"
#include "ModelBase/headers/Model.h"

#include <QtGui/QApplication>
#include <QtCore/QEvent>

namespace Visualization {

ModelRenderer Scene::defaultRenderer_;

class UpdateSceneEvent : public QEvent
{
	public:
		static const QEvent::Type EventType = (QEvent::Type) (User + 1);
		UpdateSceneEvent() : QEvent(EventType){};
};

Scene::Scene() : QGraphicsScene(VisualizationManager::instance().getMainWindow()), needsUpdate(false), renderer_(NULL), sceneHandlerItem_(new SceneHandlerItem(this))
{
	// TODO Auto-generated constructor stub
}

Scene::~Scene()
{
	for (int i = 0; i<topLevelItems.size(); ++i) SAFE_DELETE_ITEM(topLevelItems[i]);
	topLevelItems.clear();
	for (int i = 0; i<selections.size(); ++i) SAFE_DELETE_ITEM(selections[i]);
	selections.clear();
	SAFE_DELETE_ITEM(sceneHandlerItem_);
	SAFE_DELETE(renderer_);
}

void Scene::addTopLevelItem(Item* item)
{
	topLevelItems.append(item);
	addItem(item);
}

void Scene::removeTopLevelItem(Item* item)
{
	topLevelItems.removeAll(item);
	removeItem(item);
}

void Scene::scheduleUpdate()
{
	if (!needsUpdate)
	{
		needsUpdate = true;
		QApplication::postEvent(this, new UpdateSceneEvent());
	}
}

void Scene::listenToModel(Model::Model* model)
{
	connect(model, SIGNAL(nodesModified(QList<Node*>)), this,  SLOT(nodesUpdated(QList<Node*>)), Qt::QueuedConnection);
}

void Scene::nodesUpdated(QList<Node*> nodes)
{
	// TODO implement this in a more efficient way.

	QList<QGraphicsItem*> list = items();
	for (int i = 0; i < list.size(); ++i)
	{
		Item* item = static_cast<Item*> (list[i]);
		if (item->hasNode() && nodes.contains(item->node())) item->setUpdateNeeded();
	}

	scheduleUpdate();
}

void Scene::customEvent(QEvent *event)
{
	if ( event->type() == UpdateSceneEvent::EventType )
	{
		// Update Top level items
		for (int i = 0; i<topLevelItems.size(); ++i) topLevelItems.at(i)->updateSubtree();

		// Update Selections
		// TODO do not recreate all items all the time.
		for (int i = 0; i<selections.size(); i++) SAFE_DELETE_ITEM(selections[i]);
		QList<QGraphicsItem *> selected = selectedItems();
		for (int i = 0; i<selected.size(); ++i)
		{
			Item* s = static_cast<Item*> (selected[i]);
			selections.append(new SelectedItem(s));
			addItem(selections.last());
			selections.last()->updateSubtree();
		}
		needsUpdate = false;
	}
	else
		QGraphicsScene::customEvent(event);
}

bool Scene::event(QEvent *event)
{
	if (event->type() != UpdateSceneEvent::EventType &&
		 event->type() != QEvent::MetaCall  &&
		 event->type() != QEvent::GraphicsSceneMouseMove &&
		 event->type() !=QEvent::GraphicsSceneHoverMove
		)
		scheduleUpdate();

	return QGraphicsScene::event(event);
}


}
