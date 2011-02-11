/***********************************************************************************************************************
 * HPositionLayout.cpp
 *
 *  Created on: Feb 3, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HPositionLayout.h"

#include "VisualizationBase/headers/layouts/PositionLayout.h"
#include "VisualizationBase/headers/Scene.h"
#include "ModelBase/headers/Model.h"

namespace Interaction {

HPositionLayout::HPositionLayout() : originalX(0), originalY(0), currentItem(NULL), currentItemPosition(NULL)
{
}

HPositionLayout* HPositionLayout::instance()
{
	static HPositionLayout h;
	return &h;
}

void HPositionLayout::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier)
	{
		Visualization::PositionLayout* layout = static_cast<Visualization::PositionLayout*> (target);

		// Find out which of the children has the cursor.
		// If there is only child or there is no child selected do not allow movement and ignore this event.
		Visualization::ModelItem* itemToMove = NULL;
		if (layout->length() >=2)
		{
			for(int i = 0; i<layout->length(); ++i)
			{
				itemToMove = layout->at<Visualization::ModelItem>(i);
				if (itemToMove->contains(itemToMove->mapFromParent(event->pos()))) break;
				else itemToMove = NULL;
			}
		}

		if (itemToMove)
		{
			Model::ExtendableNode* ext = static_cast<Model::ExtendableNode*> (itemToMove->getNode());
			Visualization::Position* pos = ext->extension<Visualization::Position>();

			target->scene()->clearSelection();
			target->scene()->clearFocus();
			itemToMove->setSelected(true);

			originalX = pos->x();
			originalY = pos->y();
			currentItem = itemToMove;
			currentItemPosition = pos;
		}
		else event->ignore();
	}
	else event->ignore();//GenericHandler::mousePressEvent(target, event);
}

void HPositionLayout::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->modifiers() == Qt::ControlModifier && !event->buttonDownPos( Qt::LeftButton).isNull() )
	{
		if (currentItem)
		{
			Visualization::PositionLayout* layout = static_cast<Visualization::PositionLayout*> (target);

			int newX = layout->toGrid( originalX + event->pos().x() - event->buttonDownPos(Qt::LeftButton).x() );
			int newY = layout->toGrid( originalY + event->pos().y() - event->buttonDownPos(Qt::LeftButton).y() );;

			if (newX != currentItemPosition->x() || newY != currentItemPosition->y())
			{
				currentItem->getNode()->model()->beginModification(currentItem->getNode(),"Change position");
				currentItemPosition->setX( newX );
				currentItemPosition->setY( newY );
				currentItem->getNode()->model()->endModification();
				currentItem->setUpdateNeeded();
				target->scene()->scheduleUpdate();
			}
		}
	}
	else GenericHandler::mouseMoveEvent(target, event);
}

void HPositionLayout::mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->modifiers() == Qt::ControlModifier && !event->button() == Qt::LeftButton)
	{
		currentItem = NULL;
		currentItemPosition = NULL;
	}
	else GenericHandler::mouseReleaseEvent(target, event);
}

}
