/***********************************************************************************************************************
 * VisualizationManager.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "VisualizationManager.h"
#include "View.h"

namespace Visualization {

VisualizationManager VisualizationManager::theInstance = VisualizationManager();

VisualizationManager::VisualizationManager() :
	envisionManager(NULL)
{
}

VisualizationManager::VisualizationManager(const VisualizationManager&)
{
}

VisualizationManager& VisualizationManager::operator =(const VisualizationManager&)
{
	return theInstance;
}

VisualizationManager& VisualizationManager::instance()
{
	return theInstance;
}

void VisualizationManager::init(Envision::EnvisionManager *manager)
{
	theInstance.envisionManager = manager;
}

QWidget* VisualizationManager::getMainWindow()
{
	return envisionManager->getMainWindow();
}

void VisualizationManager::addTopLevelView(View* view)
{
	views.append(view);

	// If this is the only view maximize it
	if (views.size() == 1)
	{
		envisionManager->getMainWindow()->setCentralWidget(view);
	}
}

}
