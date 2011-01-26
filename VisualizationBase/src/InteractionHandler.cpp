/***********************************************************************************************************************
 * InteractionHandler.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

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

// Event filters,
void InteractionHandler::beforeEvent(Item *, QEvent*) {}
void InteractionHandler::afterEvent(Item *, QEvent*) {}

// Command events
void InteractionHandler::command(Item *, const QString&) {}

// Keyboard events
void InteractionHandler::keyPressEvent(Item *target, QKeyEvent *event) { target->defaultKeyPressEvent(event); }
void InteractionHandler::keyReleaseEvent(Item *target, QKeyEvent *event) { target->defaultKeyReleaseEvent(event); }

// Mouse events
void InteractionHandler::mouseDoubleClickEvent(Item *target, QGraphicsSceneMouseEvent *event) { target->defaultMouseDoubleClickEvent(event); }
void InteractionHandler::mousePressEvent(Item *target, QGraphicsSceneMouseEvent *event) { target->defaultMousePressEvent(event); }
void InteractionHandler::mouseMoveEvent(Item *target, QGraphicsSceneMouseEvent *event) { target->defaultMouseMoveEvent(event); }
void InteractionHandler::mouseReleaseEvent(Item *target, QGraphicsSceneMouseEvent *event) { target->defaultMouseReleaseEvent(event); }
void InteractionHandler::wheelEvent(Item *target, QGraphicsSceneWheelEvent *event) { target->defaultWheelEvent(event); }

void InteractionHandler::hoverEnterEvent(Item *target, QGraphicsSceneHoverEvent *event) { target->defaultHoverEnterEvent(event); }
void InteractionHandler::hoverLeaveEvent(Item *target, QGraphicsSceneHoverEvent *event) { target->defaultHoverLeaveEvent(event); }
void InteractionHandler::hoverMoveEvent(Item *target, QGraphicsSceneHoverEvent *event) { target->defaultHoverMoveEvent(event); }

void InteractionHandler::dragEnterEvent(Item *target, QGraphicsSceneDragDropEvent *event) { target->defaultDragEnterEvent(event); }
void InteractionHandler::dragLeaveEvent(Item *target, QGraphicsSceneDragDropEvent *event) { target->defaultDragLeaveEvent(event); }
void InteractionHandler::dragMoveEvent(Item *target, QGraphicsSceneDragDropEvent *event) { target->defaultDragMoveEvent(event); }
void InteractionHandler::dropEvent(Item *target, QGraphicsSceneDragDropEvent *event) { target->defaultDropEvent(event); }

// Menu events
void InteractionHandler::contextMenuEvent(Item *target, QGraphicsSceneContextMenuEvent *event) { target->defaultContextMenuEvent(event); }

// Focus events
void InteractionHandler::focusInEvent(Item *target, QFocusEvent *event) { target->defaultFocusInEvent(event); }
void InteractionHandler::focusOutEvent(Item *target, QFocusEvent *event) { target->defaultFocusOutEvent(event); }

}
