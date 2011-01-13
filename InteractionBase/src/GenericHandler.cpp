/***********************************************************************************************************************
 * GenericHandler.cpp
 *
 *  Created on: Jan 13, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "GenericHandler.h"

#include "VisualizationBase/headers/Scene.h"

namespace Interaction {

GenericHandler::GenericHandler()
{
}

GenericHandler* GenericHandler::instance()
{
	static GenericHandler h;
	return &h;
}

void GenericHandler::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		QPainterPath path;
		path.addRect( QRectF(event->scenePos(), event->scenePos()).adjusted(0,0,1,1) );
		target->scene()->setSelectionArea(path, Qt::IntersectsItemShape);
	}
}

void GenericHandler::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (!event->buttonDownPos( Qt::LeftButton).isNull())
	{
		QPainterPath path;
		path.addRect( QRectF(event->buttonDownScenePos(Qt::LeftButton), event->scenePos()) );
		target->scene()->setSelectionArea(path, Qt::IntersectsItemShape);
	}

	InteractionHandler::mouseMoveEvent(target, event);
}

}
