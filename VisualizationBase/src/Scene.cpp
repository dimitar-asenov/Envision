/***********************************************************************************************************************
 * Scene.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Scene.h"
#include "VisualizationManager.h"

namespace Visualization {

Scene::Scene() : QGraphicsScene(VisualizationManager::instance().getMainWindow())
{
}

Scene::~Scene()
{
	// TODO Auto-generated destructor stub
}

}
