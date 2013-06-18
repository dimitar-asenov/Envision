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

#pragma once

#include "visualizationbase_api.h"

namespace Visualization {

class Item;

class VISUALIZATIONBASE_API InteractionHandler
{
	protected:
		InteractionHandler();

	public:

		// Event filters,
		virtual void beforeEvent(Item *target, QEvent* event);
		virtual void afterEvent(Item *target, QEvent* event);

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
		virtual void command(Item *target, const QString& command); // Introduced by Envision

		// Destructor
		virtual ~InteractionHandler();

		// Instance
		static InteractionHandler* instance();

		static bool isClick(Item* target);
		static bool isDrag(Item* target);
};

}
