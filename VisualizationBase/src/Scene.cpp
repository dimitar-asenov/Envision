/***********************************************************************************************************************
 * Scene.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Scene.h"
#include "VisualizationManager.h"
#include "items/Item.h"

namespace Visualization {

Scene::Scene() : QGraphicsScene(VisualizationManager::instance().getMainWindow()), renderer_(NULL)
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
	for (int i = 0; i<topLevelItems.size(); ++i) topLevelItems.at(i)->updateSubtreeState();
}

}
