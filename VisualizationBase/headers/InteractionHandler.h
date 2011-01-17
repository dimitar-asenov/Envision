/***********************************************************************************************************************
 * InteractionHandler.h
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef INTERACTIONHANDLER_H_
#define INTERACTIONHANDLER_H_

#include "visualizationbase_api.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsSceneWheelEvent>
#include <QtGui/QGraphicsSceneHoverEvent>
#include <QtGui/QGraphicsSceneDragDropEvent>
#include <QtGui/QGraphicsSceneContextMenuEvent>
#include <QtGui/QFocusEvent>

namespace Visualization {

class Item;

class VISUALIZATIONBASE_API InteractionHandler
{
	protected:
		InteractionHandler();

	public:

		// Keyboard events
		virtual void keyPressEvent(Item *target, QKeyEvent *event);
		virtual void keyReleaseEvent(Item *target, QKeyEvent *event);

		// Mouse events
		virtual void mouseDoubleClickEvent(Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mousePressEvent(Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseReleaseEvent(Item *target, QGraphicsSceneMouseEvent *event);
		virtual void wheelEvent(Item *target, QGraphicsSceneWheelEvent *event);

		virtual void hoverEnterEvent(Item *target, QGraphicsSceneHoverEvent *event);
		virtual void hoverLeaveEvent(Item *target, QGraphicsSceneHoverEvent *event);
		virtual void hoverMoveEvent(Item *target, QGraphicsSceneHoverEvent *event);

		virtual void dragEnterEvent(Item *target, QGraphicsSceneDragDropEvent *event);
		virtual void dragLeaveEvent(Item *target, QGraphicsSceneDragDropEvent *event);
		virtual void dragMoveEvent(Item *target, QGraphicsSceneDragDropEvent *event);
		virtual void dropEvent(Item *target, QGraphicsSceneDragDropEvent *event);

		// Menu events
		virtual void contextMenuEvent(Item *target, QGraphicsSceneContextMenuEvent *event);

		// Focus events
		virtual void focusInEvent(Item *target, QFocusEvent *event);
		virtual void focusOutEvent(Item *target, QFocusEvent *event);

		// Command events
		virtual void command(Item *target, const QString& command);

		// Destructor
		virtual ~InteractionHandler();

		// Instance
		static InteractionHandler* instance();
};

}

#endif /* INTERACTIONHANDLER_H_ */
