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

#include "SetCursorEvent.h"

#include "InteractionBase/src/prompt/Prompt.h"
#include "VisualizationBase/src/items/Item.h"

namespace Interaction {

const QEvent::Type SetCursorEvent::EventType = static_cast<QEvent::Type> (QEvent::registerEventType());

SetCursorEvent::SetCursorEvent(Visualization::Item* itemToGetCursor, CursorPlacement placement, bool showPrompt)
: CustomSceneEvent{EventType}, itemToGetCursor_{itemToGetCursor}, placement_{placement}, showPrompt_{showPrompt}
{}

SetCursorEvent::SetCursorEvent(Visualization::Item* itemToGetCursor, QPoint point, bool showPrompt)
: CustomSceneEvent{EventType}, itemToGetCursor_{itemToGetCursor}, point_{point}, showPrompt_{showPrompt}
{}

SetCursorEvent::SetCursorEvent(GetItemFunction getItemToFocus, CursorPlacement placement, bool showPrompt)
: CustomSceneEvent{EventType}, getItemToFocus_{getItemToFocus}, placement_{placement}, showPrompt_{showPrompt}
{}

SetCursorEvent::SetCursorEvent(GetItemFunction getItemToFocus, GetCursorPlacement getCursorPlacement, bool showPrompt)
: CustomSceneEvent{EventType}, getItemToFocus_{getItemToFocus}, getCursorPlacement_{getCursorPlacement},
	showPrompt_{showPrompt}
{}

SetCursorEvent::SetCursorEvent(Visualization::Item* parentContainer, Model::Node* node, CursorPlacement placement,
		bool showPrompt)
	: CustomSceneEvent{EventType}, parentContainer_{parentContainer}, node_{node}, placement_{placement},
	  showPrompt_{showPrompt}
{}

SetCursorEvent::SetCursorEvent(Visualization::Scene* scene, Model::Node* node, CursorPlacement placement,
		bool showPrompt)
	: CustomSceneEvent{EventType}, scene_{scene}, node_{node},  placement_{placement}, showPrompt_{showPrompt}
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
		for (auto i : scene_->topLevelItems())
		{
			item = i->findVisualizationOf(node_);
			if (item) break;
		}
	}

	if (!item) return;

	if (point_.x() >= 0 && point_.y() >= 0)
	{
		item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition, point_);
	}
	else
	{
		auto parent = item->parent();

		if (getCursorPlacement_) placement_ = getCursorPlacement_();
		switch (placement_)
		{
			case CursorOnTop: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnTop); break;
			case CursorOnBottom: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnBottom); break;
			case CursorOnLeft: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnLeft); break;
			case CursorOnRight: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnRight); break;
			case CursorOnCenter: item->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnCenter); break;

			case CursorAboveOf: parent->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
					item->pos().toPoint() + QPoint{item->widthInParent() / 2, 0}); break;
			case CursorBelowOf: parent->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
					item->pos().toPoint() + QPoint{item->widthInParent() / 2, item->heightInParent()-1}); break;
			case CursorLeftOf: parent->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
					item->pos().toPoint() + QPoint{0, item->heightInParent() / 2}); break;
			case CursorRightOf: parent->moveCursor(Visualization::Item::CursorMoveDirection::MoveOnPosition,
					item->pos().toPoint() + QPoint{item->widthInParent() - 1, item->heightInParent() / 2}); break;
			case CursorDefault: item->moveCursor(); break;

			default: item->moveCursor(); break;
		}
	}

	if (showPrompt_)
	{
		auto it = item->scene()->focusItem();
		if (it)
		{
			// This indirection (add a new event) is necessary because otherwise, something goes wrong with the view
			// update and there are some pixels which are not cleared and still remain after the new prompt has been
			// shown. It is not clear what the problem exactly is and where it should be fixed but the workaround below
			// works.
			// TODO: Investigate the "dirty" pixels further
			qApp->postEvent(item->scene(), new CustomSceneEvent{[=](){ Prompt::show(it); }});
		}
	}

}

}
