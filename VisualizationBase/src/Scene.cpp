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

#include <QtGui/QApplication>
#include <QtCore/QEvent>

namespace Visualization {

class UpdateSceneEvent : public QEvent
{
	public:
		static const QEvent::Type EventType = (QEvent::Type) (User + 1);
		UpdateSceneEvent() : QEvent(EventType){};
};

Scene::Scene() : QGraphicsScene(VisualizationManager::instance().getMainWindow()), renderer_(NULL), sceneHandlerItem_(new SceneHandlerItem(this))
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

void Scene::updateTopLevelItems()
{
	QApplication::postEvent(this, new UpdateSceneEvent());
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
	}
	else
		QGraphicsScene::customEvent(event);
}


}
