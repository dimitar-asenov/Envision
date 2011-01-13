/***********************************************************************************************************************
 * MainView.cpp
 *
 *  Created on: Jan 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "views/MainView.h"
#include "Scene.h"

#include "Core/headers/global.h"
#include <QtGui/QWheelEvent>

namespace Visualization {

MainView::MainView(Scene *scene) :
	View(scene, NULL), miniMap(new MiniMap(scene, this)), scaleLevel(SCALING_FACTOR)
{
	setRenderHint(QPainter::Antialiasing);
	setRenderHint(QPainter::TextAntialiasing);

	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	setMiniMapSize(MINIMAP_DEFAULT_WIDTH, MINIMAP_DEFAULT_HEIGHT);
}

MainView::~MainView()
{
	SAFE_DELETE(miniMap);
}

void MainView::setMiniMapSize(int width, int height)
{
	if ( miniMap )
	{
		miniMap->resize(width, height);
		miniMap->sceneRectChanged(scene()->sceneRect());
	}
}

void MainView::resizeEvent(QResizeEvent *event)
{
	View::resizeEvent(event);
	if ( miniMap )
	{
		miniMap->updatePosition();
		miniMap->visibleRectChanged();
	}
}

void MainView::wheelEvent(QWheelEvent *event)
{
	if ( event->delta() > 0 ) scaleLevel--;
	else scaleLevel++;

	if ( scaleLevel <= 0 ) scaleLevel = 1;
	qreal scaleFactor = SCALING_FACTOR / (qreal) scaleLevel;
	setTransform(QTransform::fromScale(scaleFactor, scaleFactor));

	if ( miniMap ) miniMap->visibleRectChanged();
}

void MainView::scrollContentsBy(int dx, int dy)
{
	View::scrollContentsBy(dx, dy);
	if ( miniMap ) miniMap->visibleRectChanged();
}

}
