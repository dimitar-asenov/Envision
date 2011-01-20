/***********************************************************************************************************************
 * View.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "views/View.h"
#include "Scene.h"
#include "VisualizationManager.h"

#include <QtGui/QScrollBar>

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

QRectF View::visibleRect()
{
	return mapToScene(rect()).boundingRect();
}

Scene* View::scene()
{
	return static_cast<Scene*> (QGraphicsView::scene());
}

}
