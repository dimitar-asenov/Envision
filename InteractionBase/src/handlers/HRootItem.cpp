/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "HRootItem.h"
#include "../events/SetCursorEvent.h"

#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"
#include "FilePersistence/src/SystemClipboard.h"
#include "ModelBase/src/nodes/List.h"

namespace Interaction {

HRootItem::HRootItem()
{

}

HRootItem* HRootItem::instance()
{
	static HRootItem h;
	return &h;
}

void HRootItem::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_F2)
	{
		event->accept();
		auto ri = static_cast<Visualization::RootItem*> (target);

		ri->setPurpose( (ri->purpose() + 1) % target->scene()->renderer()->numRegisteredPurposes() );

		target->scene()->addPostEventAction(
			new SetCursorEvent{target, ri->node()});
	}
	else GenericHandler::keyPressEvent(target, event);
}


void HRootItem::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ShiftModifier)
	{
		offset = (target->pos() - event->scenePos()).toPoint();
	}
	else event->ignore();//GenericHandler::mousePressEvent(target, event);
}

void HRootItem::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	target->setPos(event->scenePos() + offset);
}

}
