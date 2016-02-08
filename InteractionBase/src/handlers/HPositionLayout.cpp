/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "HPositionLayout.h"

#include "VisualizationBase/src/layouts/PositionLayout.h"
#include "VisualizationBase/src/Scene.h"
#include "ModelBase/src/model/TreeManager.h"

namespace Interaction {

HPositionLayout::HPositionLayout() : originalX{0}, originalY{0}, currentItem{}, currentItemPosition{}
{
}

HPositionLayout* HPositionLayout::instance()
{
	static HPositionLayout h;
	return &h;
}

void HPositionLayout::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ShiftModifier)
	{
		Visualization::PositionLayout* layout = static_cast<Visualization::PositionLayout*> (target);

		// Find out which of the children has the cursor.
		// If there is only child or there is no child selected do not allow movement and ignore this event.
		Visualization::Item* itemToMove = nullptr;
		if (layout->length() >=2)
		{
			for (int i = 0; i<layout->length(); ++i)
			{
				itemToMove = layout->at<Visualization::Item>(i);
				if (itemToMove->contains(itemToMove->mapFromParent(event->pos()))) break;
				else itemToMove = nullptr;
			}
		}

		if (itemToMove)
		{
			Model::CompositeNode* composite = static_cast<Model::CompositeNode*> (itemToMove->node());
			auto pos = composite->extension<Visualization::Position>();

			target->scene()->clearSelection();
			target->scene()->clearFocus();
			itemToMove->setSelected(true);

			originalX = pos->xNode() ? pos->x() : 0;
			originalY = pos->yNode() ? pos->y() : 0;
			currentItem = itemToMove;
			currentItemPosition.swap(pos);
		}
		else event->ignore();
	}
	else event->ignore();//GenericHandler::mousePressEvent(target, event);
}

void HPositionLayout::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (currentItem)
	{
		Visualization::PositionLayout* layout = static_cast<Visualization::PositionLayout*> (target);

		int newX = layout->toGrid( originalX + event->pos().x() - event->buttonDownPos(Qt::LeftButton).x() );
		int newY = layout->toGrid( originalY + event->pos().y() - event->buttonDownPos(Qt::LeftButton).y() );;

		int currentItemPositionX = currentItemPosition->xNode() ? currentItemPosition->x() : 0;
		int currentItemPositionY = currentItemPosition->yNode() ? currentItemPosition->y() : 0;
		if (newX != currentItemPositionX || newY != currentItemPositionY)
		{
			currentItem->node()->manager()->beginModification(currentItem->node(), "Change position");
			currentItemPosition->setX( newX );
			currentItemPosition->setY( newY );
			currentItem->node()->manager()->endModification();
			currentItem->setUpdateNeeded(Visualization::Item::StandardUpdate);
			target->scene()->scheduleUpdate();
		}
	}
}

void HPositionLayout::mouseReleaseEvent(Visualization::Item *, QGraphicsSceneMouseEvent *)
{
	currentItem = nullptr;
	currentItemPosition = nullptr;
}

}
