/***********************************************************************************************************************
 * View.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "View.h"
#include "Scene.h"
#include "VisualizationManager.h"

namespace Visualization {

View::View(Scene* scene, View *parent) :
	QGraphicsView(scene, parent)
{
	if ( parent == NULL )
	{
		setParent(VisualizationManager::instance().getMainWindow());
		VisualizationManager::instance().addTopLevelView(this);
	}
}

View::~View()
{
}

}
