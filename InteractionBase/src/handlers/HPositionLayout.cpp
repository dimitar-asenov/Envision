/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

/***********************************************************************************************************************
 * HPositionLayout.cpp
 *
 *  Created on: Feb 3, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HPositionLayout.h"

#include "VisualizationBase/src/layouts/PositionLayout.h"
#include "VisualizationBase/src/Scene.h"
#include "ModelBase/src/Model.h"

namespace Interaction {

HPositionLayout::HPositionLayout() : originalX(0), originalY(0), currentItem(nullptr), currentItemPosition(nullptr)
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
		Visualization::Item* itemToMove = nullptr;
		if (layout->length() >=2)
		{
			for(int i = 0; i<layout->length(); ++i)
			{
				itemToMove = layout->at<Visualization::Item>(i);
				if (itemToMove->contains(itemToMove->mapFromParent(event->pos()))) break;
				else itemToMove = nullptr;
			}
		}

		if (itemToMove)
		{
			Model::ExtendableNode* ext = static_cast<Model::ExtendableNode*> (itemToMove->node());
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
				currentItem->node()->model()->beginModification(currentItem->node(),"Change position");
				currentItemPosition->setX( newX );
				currentItemPosition->setY( newY );
				currentItem->node()->model()->endModification();
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
		currentItem = nullptr;
		currentItemPosition = nullptr;
	}
	else GenericHandler::mouseReleaseEvent(target, event);
}

}
