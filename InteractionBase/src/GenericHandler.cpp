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

		// Filter out items for which the selection is completely internal.
		QList<QGraphicsItem*> selection = target->scene()->selectedItems();
		for (int i = 0; i<selection.size(); ++i)
		{
			if (  selection.at(i)->contains(selection.at(i)->mapFromScene(event->buttonDownScenePos(Qt::LeftButton)))
				&& selection.at(i)->contains(selection.at(i)->mapFromScene(event->scenePos())))
				selection.at(i)->setSelected(false);
		}

		// Filter out items whose parent is also included in the selection.
		selection = target->scene()->selectedItems();
		for (int i = 0; i<selection.size(); ++i)
		{
			QGraphicsItem* parent = selection.at(i)->parentItem();
			while (parent)
			{
				if (selection.contains(parent))
				{
					selection.at(i)->setSelected(false);
					break;
				}
				parent = parent->parentItem();
			}
		}

	}

	InteractionHandler::mouseMoveEvent(target, event);
}

}
