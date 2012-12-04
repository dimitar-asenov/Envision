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
 * HList.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HList.h"
#include "events/SetCursorEvent.h"

#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"
#include "FilePersistence/src/SystemClipboard.h"
#include "ModelBase/src/nodes/List.h"

namespace Interaction {

HList::HList()
{

}

HList* HList::instance()
{
	static HList h;
	return &h;
}

void HList::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	Visualization::VList* list = static_cast<Visualization::VList*> (target);
	bool processed = false;

	if (event->matches(QKeySequence::Paste) && list->scene()->mainCursor()->owner() == list->layout())
	{
		processed = true;
		FilePersistence::SystemClipboard clipboard;
		int selIndex = list->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
		if (selIndex > list->length()) selIndex = list->length();
		if (selIndex < 0) selIndex = 0;

		if (clipboard.numNodes() > 0)
		{
			Model::List* l = dynamic_cast<Model::List*> (list->node());
			FilePersistence::SystemClipboard clipboard;
			l->model()->beginModification(l,"paste into list");
			if (l) l->paste(clipboard, selIndex);
			l->model()->endModification();
			target->setUpdateNeeded(Visualization::Item::StandardUpdate);
		}
	}
	else if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)  && event->key() == Qt::Key_V)
	{
		processed = true;
		FilePersistence::SystemClipboard clipboard;
		int selIndex = list->focusedElementIndex();

		if (clipboard.numNodes() > 0 && selIndex >= 0)
		{
			Model::List* l = dynamic_cast<Model::List*> (list->node());
			FilePersistence::SystemClipboard clipboard;
			l->model()->beginModification(l,"paste");
			if (l) l->paste(clipboard, selIndex+1);
			l->model()->endModification();
			target->setUpdateNeeded(Visualization::Item::StandardUpdate);
		}
	}

	if (!list->suppressDefaultRemovalHandler() && event->modifiers() == Qt::NoModifier
			&& (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
			&& list->scene()->mainCursor() && list->scene()->mainCursor()->owner() == list->layout())
	{
		processed = true;

		int index = list->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
		if (event->key() == Qt::Key_Backspace) --index;
		// Delete the element corresponding to index

		if (index >=0 && index < list->layout()->length())
			removeAndSetCursor(list, index);

	}

	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
	{
		bool createDown = event->modifiers() == Qt::NoModifier;
		bool createRight = event->modifiers() == Qt::ShiftModifier;

		if ((createDown && !list->layout()->style()->isHorizontal())
				|| ( createRight && list->layout()->style()->isHorizontal()) )
		{
			auto newElem = list->node()->createDefaultElement();
			if (newElem)
			{
				processed = true;

				int index = -1;
				if (list->scene()->mainCursor() && list->scene()->mainCursor()->owner() == list->layout())
					index = list->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
				else
				{
					index = list->focusedElementIndex();
					if (index >= 0) ++index;
				}

				if (index >= 0)
				{
					list->node()->model()->beginModification(list->node(), "add new list element");
					list->node()->insert(index, newElem);
					list->node()->model()->endModification();

					list->setUpdateNeeded(Visualization::Item::StandardUpdate);
						target->scene()->addPostEventAction( new SetCursorEvent(list, newElem,
							createDown ? SetCursorEvent::CursorOnLeft : SetCursorEvent::CursorOnTop));
				}
			}
		}
	}


	if (!processed) GenericHandler::keyPressEvent(target, event);
}

void HList::scheduleSetCursor(Visualization::VList* list, Model::Node* listItemToSelect,
		SetCursorEvent::CursorPlacement howToSelectItem)
{
	list->scene()->addPostEventAction( new SetCursorEvent(list, listItemToSelect, howToSelectItem));
}

void HList::scheduleSetCursor(Visualization::VList* list, int setCursorIndex)
{
	Q_ASSERT(setCursorIndex >= 0 && setCursorIndex <= list->length());

	if (setCursorIndex == 0)
		list->scene()->addPostEventAction(
			new SetCursorEvent([=](){return list->length() ? list->at<Visualization::Item>(0) : list;},
					(list->layout()->isHorizontal() ? SetCursorEvent::CursorLeftOf : SetCursorEvent::CursorAboveOf)));
	else
		list->scene()->addPostEventAction(
			new SetCursorEvent([=](){return list->at<Visualization::Item>(setCursorIndex-1);},
					(list->layout()->isHorizontal() ? SetCursorEvent::CursorRightOf : SetCursorEvent::CursorBelowOf)));
}

void HList::removeAndSetCursor(Visualization::VList* list, int removeAt, bool setCursorDown,
				SetCursorEvent::CursorPlacement howToSelectItem)
{
	auto node = list->node();
	Q_ASSERT(removeAt >= 0 && removeAt < node->size());

	node->beginModification("Remove list item");
	node->remove(removeAt);
	node->endModification();
	list->setUpdateNeeded(Visualization::Item::StandardUpdate);

	if (!setCursorDown) --removeAt;
	if (removeAt >=0 && removeAt < node->size())
		scheduleSetCursor(list, node->at<Model::Node>(removeAt), howToSelectItem);
	else
		scheduleSetCursor(list, removeAt < 0 ? 0 : removeAt);
}

void HList::removeAndSetCursor(Visualization::VList* list, int removeAt)
{
	auto node = list->node();
	Q_ASSERT(removeAt >= 0 && removeAt < node->size());

	node->beginModification("Remove list item");
	node->remove(removeAt);
	node->endModification();
	list->setUpdateNeeded(Visualization::Item::StandardUpdate);

	scheduleSetCursor(list, removeAt);
}

void HList::removeAndSetCursor(Visualization::VList* list, Model::Node* removeNode, bool setCursorDown,
			SetCursorEvent::CursorPlacement howToSelectItem)
{
	removeAndSetCursor(list, list->node()->indexOf(removeNode), setCursorDown, howToSelectItem);
}

void HList::removeAndSetCursor(Visualization::VList* list, Model::Node* removeNode)
{
	removeAndSetCursor(list, list->node()->indexOf(removeNode));
}

}
