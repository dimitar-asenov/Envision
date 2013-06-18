/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "InteractionHandler.h"
#include "items/Item.h"
#include "Scene.h"

namespace Visualization {

InteractionHandler::InteractionHandler()
{
}

InteractionHandler::~InteractionHandler()
{
}

InteractionHandler* InteractionHandler::instance()
{
	static InteractionHandler h;
	return &h;
}

bool InteractionHandler::isClick(Item* target)
{
	return target->scene()->isCurrentMousePressAClick();
}


bool InteractionHandler::isDrag(Item* target)
{
	return !target->scene()->isCurrentMousePressAClick();
}

// Event filters,
void InteractionHandler::beforeEvent(Item *, QEvent*) {}
void InteractionHandler::afterEvent(Item *, QEvent*) {}

// Command events
void InteractionHandler::command(Item *, const QString&) {}

// Keyboard events
void InteractionHandler::keyPressEvent(Item *target, QKeyEvent *event)
{
	target->defaultKeyPressEvent(event);
}
void InteractionHandler::keyReleaseEvent(Item *target, QKeyEvent *event)
{
	target->defaultKeyReleaseEvent(event);
}

// Mouse events
void InteractionHandler::mouseDoubleClickEvent(Item *target, QGraphicsSceneMouseEvent *event)
{
	target->defaultMouseDoubleClickEvent(event);
}
void InteractionHandler::mousePressEvent(Item *target, QGraphicsSceneMouseEvent *event)
{
	target->defaultMousePressEvent(event);
}
void InteractionHandler::mouseMoveEvent(Item *target, QGraphicsSceneMouseEvent *event)
{
	target->defaultMouseMoveEvent(event);
}
void InteractionHandler::mouseReleaseEvent(Item *target, QGraphicsSceneMouseEvent *event)
{
	target->defaultMouseReleaseEvent(event);
}
void InteractionHandler::wheelEvent(Item *target, QGraphicsSceneWheelEvent *event)
{
	target->defaultWheelEvent(event);
}

void InteractionHandler::hoverEnterEvent(Item *target, QGraphicsSceneHoverEvent *event)
{
	target->defaultHoverEnterEvent(event);
}
void InteractionHandler::hoverLeaveEvent(Item *target, QGraphicsSceneHoverEvent *event)
{
	target->defaultHoverLeaveEvent(event);
}
void InteractionHandler::hoverMoveEvent(Item *target, QGraphicsSceneHoverEvent *event)
{
	target->defaultHoverMoveEvent(event);
}

void InteractionHandler::dragEnterEvent(Item *target, QGraphicsSceneDragDropEvent *event)
{
	target->defaultDragEnterEvent(event);
}
void InteractionHandler::dragLeaveEvent(Item *target, QGraphicsSceneDragDropEvent *event)
{
	target->defaultDragLeaveEvent(event);
}
void InteractionHandler::dragMoveEvent(Item *target, QGraphicsSceneDragDropEvent *event)
{
	target->defaultDragMoveEvent(event);
}
void InteractionHandler::dropEvent(Item *target, QGraphicsSceneDragDropEvent *event)
{
	target->defaultDropEvent(event);
}

// Menu events
void InteractionHandler::contextMenuEvent(Item *target, QGraphicsSceneContextMenuEvent *event)
{
	target->defaultContextMenuEvent(event);
}

// Focus events
void InteractionHandler::focusInEvent(Item *target, QFocusEvent *event)
{
	target->defaultFocusInEvent(event);
}
void InteractionHandler::focusOutEvent(Item *target, QFocusEvent *event)
{
	target->defaultFocusOutEvent(event);
}

}
