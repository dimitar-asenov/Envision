/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * SetCursorEvent.cpp
 *
 *  Created on: Mar 1, 2012
 *      Author: Dimitar Asenov
 */

#include "handlers/SetCursorEvent.h"

#include "VisualizationBase/src/items/Item.h"
#include "ModelBase/src/adapter/AdapterManager.h"

namespace Interaction {

const QEvent::Type SetCursorEvent::EventType = static_cast<QEvent::Type> (QEvent::registerEventType());

SetCursorEvent::SetCursorEvent(Visualization::Item* parentContainer, Model::Node* node, CursorPlacement placement)
	: CustomSceneEvent(EventType), parentContainer_(parentContainer), node_(node), placement_(placement)
{
}

void SetCursorEvent::execute()
{
	auto item = parentContainer_->findVisualizationOf(node_);

	if (!item) item = parentContainer_;

	auto parent = static_cast<Visualization::Item*>(item->parentItem());

	switch(placement_)
	{
		case CursorOnTop: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				QPoint(item->width() / 2, 0)); break;
		case CursorOnBottom: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				QPoint(item->width() / 2, item->yEnd())); break;
		case CursorOnLeft: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				QPoint(0, item->height() / 2)); break;
		case CursorOnRight: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				QPoint(item->xEnd(), item->height() / 2)); break;
		case CursorOnCenter: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				QPoint(item->width() / 2,item->height() / 2)); break;

		case CursorAboveOf: parent->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				item->pos().toPoint() + QPoint(item->width() / 2, 0)); break;
		case CursorBelowOf: parent->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				item->pos().toPoint() + QPoint(item->width() / 2, item->height()-1)); break;
		case CursorLeftOf: parent->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				item->pos().toPoint() + QPoint(0, item->height() / 2)); break;
		case CursorRightOf: parent->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				item->pos().toPoint() + QPoint(item->width() - 1, item->height() / 2)); break;

		default: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
				QPoint(0, 0)); break;
	}
}

} /* namespace Interaction */
