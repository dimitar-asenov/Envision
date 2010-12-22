/***********************************************************************************************************************
 * Scene.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Scene.h"
#include "VisualizationManager.h"

namespace Visualization {

Scene::Scene() : QGraphicsScene(VisualizationManager::instance().getMainWindow()), renderer_(NULL)
{
	// TODO Auto-generated constructor stub
}

}
