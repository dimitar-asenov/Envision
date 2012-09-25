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

#include "SetCursorEvent.h"

#include "VisualizationBase/src/items/Item.h"
#include "ModelBase/src/adapter/AdapterManager.h"

namespace Interaction {

const QEvent::Type SetCursorEvent::EventType = static_cast<QEvent::Type> (QEvent::registerEventType());

SetCursorEvent::SetCursorEvent(Visualization::Item* itemToGetCursor, CursorPlacement placement)
: CustomSceneEvent(EventType), itemToGetCursor_(itemToGetCursor), parentContainer_(), scene_(),
  node_(), placement_(placement)
{}

SetCursorEvent::SetCursorEvent(GetItemFunction getItemToFocus, CursorPlacement placement)
: CustomSceneEvent(EventType), itemToGetCursor_(), parentContainer_(), getItemToFocus_(getItemToFocus), scene_(),
  node_(), placement_(placement)
{}

SetCursorEvent::SetCursorEvent(Visualization::Item* parentContainer, Model::Node* node, CursorPlacement placement)
	: CustomSceneEvent(EventType), itemToGetCursor_(), parentContainer_(parentContainer), scene_(),
	  node_(node), placement_(placement)
{}

SetCursorEvent::SetCursorEvent(Visualization::Scene* scene, Model::Node* node, CursorPlacement placement)
	: CustomSceneEvent(EventType), itemToGetCursor_(), parentContainer_(), scene_(scene), node_(node),
	  placement_(placement)
{}

void SetCursorEvent::execute()
{
	Visualization::Item* item = nullptr;

	if (itemToGetCursor_)
		item = itemToGetCursor_;
	else if (getItemToFocus_)
		item = getItemToFocus_();
	else if (parentContainer_)
	{
		item = parentContainer_->findVisualizationOf(node_);
		if (!item) item = parentContainer_;
	}
	else
	{
		for(auto i : scene_->topLevelItems())
		{
			item = i->findVisualizationOf(node_);
			if (item) break;
		}
	}

	if (!item) return;
	auto parent = item->parent();

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
		case CursorDefault: item->moveCursor(); break;

		default: item->moveCursor(); break;
	}
}

} /* namespace Interaction */
