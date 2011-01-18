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
		for (int i = 0; i<topLevelItems.size(); ++i) topLevelItems.at(i)->updateSubtree();
	}
	else
		QGraphicsScene::customEvent(event);
}


}
