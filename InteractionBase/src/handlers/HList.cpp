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

#include "HList.h"
#include "../events/SetCursorEvent.h"

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

void HList::focusOutEvent(Visualization::Item *target, QFocusEvent *event)
{
	Visualization::VList* list = static_cast<Visualization::VList*> (target);
	if (list->node()->isEmpty() && list->style()->showTipWhenSelectedAndEmpty())
		list->setUpdateNeeded(Visualization::Item::StandardUpdate);
	GenericHandler::focusOutEvent(target, event);
}

void HList::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	Visualization::VList* list = static_cast<Visualization::VList*> (target);
	bool processed = false;

	if (event->matches(QKeySequence::Paste) && list->scene()->mainCursor()->owner() == list &&
		 !list->ignoresCopyAndPaste())
	{
		processed = true;
		FilePersistence::SystemClipboard clipboard;
		int selIndex = list->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
		if (selIndex > list->length()) selIndex = list->length();
		if (selIndex < 0) selIndex = 0;
		selIndex += list->rangeBegin();

		if (clipboard.numNodes() > 0)
		{
			Model::List* listNode = DCast<Model::List> (list->node());
			FilePersistence::SystemClipboard clipboard;
			listNode->beginModification("paste into list");
			if (listNode) listNode->paste(clipboard, selIndex);
			listNode->endModification();
			target->setUpdateNeeded(Visualization::Item::StandardUpdate);
		}
	}
	else if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && event->key() == Qt::Key_V &&
				!list->ignoresCopyAndPaste())
	{
		processed = true;
		FilePersistence::SystemClipboard clipboard;
		int selIndex = list->focusedNodeIndex();

		if (clipboard.numNodes() > 0 && selIndex >= 0)
		{
			Model::List* listNode = DCast<Model::List> (list->node());
			FilePersistence::SystemClipboard clipboard;
			listNode->beginModification("paste into list");
			if (listNode) listNode->paste(clipboard, selIndex+1);
			listNode->endModification();
			target->setUpdateNeeded(Visualization::Item::StandardUpdate);
		}
	}

	if (!list->suppressDefaultRemovalHandler() && event->modifiers() == Qt::NoModifier
			&& (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
			&& list->scene()->mainCursor() && list->scene()->mainCursor()->owner() == list)
	{
		processed = true;

		int index = list->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
		if (event->key() == Qt::Key_Backspace) --index;

		// Delete the node corresponding to index. Make sure not to delete invisible nodes, that this list is currently
		// not displaying. Check the boundaries on the layout, not on the nodes() array.
		if (index >=0 && index < list->length())
			removeNodeAndSetCursor(list, index + list->rangeBegin());
	}

	if (	event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return
			|| (event->key() == Qt::Key_Tab  && event->modifiers() == Qt::NoModifier)
			|| (	!list->suppressDefaultRemovalHandler() &&
					(event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier)
					&& !event->text().isEmpty() && event->text().at(0).isLetterOrNumber()))
	{
		bool enter = event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return;
		bool createDown = enter && event->modifiers() == Qt::NoModifier && !list->style()->itemsStyle().isHorizontal();
		bool createRight = enter && event->modifiers() == Qt::ShiftModifier && list->style()->itemsStyle().isHorizontal();

		if ( !enter || (enter && list->isShowingEmptyTip()) || createDown || createRight )
		{
			auto newElem = list->node()->createDefaultElement();
			if (newElem)
			{
				processed = true;

				int index = -1;
				if (list->isShowingEmptyTip()) index = 0;
				else if (list->scene()->mainCursor() && list->scene()->mainCursor()->owner() == list)
					index = list->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
				else
				{
					index = list->focusedItemIndex();
					if (index >= 0) ++index;
				}

				// At this point, index indicates where in the LAYOUT we should put the new element.
				if (index >= 0 && index + list->rangeBegin() <= list->node()->size())
				{
					list->node()->beginModification("add new list element");
					list->node()->insert(index + list->rangeBegin(), newElem);
					list->node()->endModification();

					list->setUpdateNeeded(Visualization::Item::StandardUpdate);
					target->scene()->addPostEventAction( new SetCursorEvent{list, newElem,
							list->style()->itemsStyle().isHorizontal()
							? SetCursorEvent::CursorOnLeft : SetCursorEvent::CursorOnTop});

					// If this was not an enter key and not a tab, then it must have been a letter.
					// Add a keyboard event for this letter
					if (!enter && event->key() != Qt::Key_Tab )
					{
						qApp->postEvent(target->scene(),  new QKeyEvent{QEvent::KeyPress, event->key(),
							event->modifiers(), event->text(), event->isAutoRepeat(), (ushort)event->count()});
					}
				}
			}
		}
	}


	if (!processed) GenericHandler::keyPressEvent(target, event);
}

void HList::scheduleSetCursor(Visualization::VList* list, Model::Node* listNodeToSelect,
		SetCursorEvent::CursorPlacement howToSelectItem)
{
	list->scene()->addPostEventAction( new SetCursorEvent{list, listNodeToSelect, howToSelectItem});
}

void HList::scheduleSetCursor(Visualization::VList* list, int setCursorNodeIndex)
{
	Q_ASSERT(setCursorNodeIndex >= 0 && setCursorNodeIndex <= list->node()->size());

	list->scene()->addPostEventAction(new SetCursorEvent{
		[=](){
			int setCursorItemIndex = 0;
			if (setCursorNodeIndex < list->rangeBegin()) setCursorItemIndex = 0;
			else if (setCursorNodeIndex > list->rangeEnd()) setCursorItemIndex = list->length();
			else setCursorItemIndex = setCursorNodeIndex - list->rangeBegin();

			if (setCursorItemIndex == 0) return list->length() ? list->itemAt<Visualization::Item>(0) : list;
			else return list->itemAt<Visualization::Item>(setCursorItemIndex-1);
		},
		[=](){
			int setCursorItemIndex = 0;
			if (setCursorNodeIndex < list->rangeBegin()) setCursorItemIndex = 0;
			else if (setCursorNodeIndex > list->rangeEnd()) setCursorItemIndex = list->length();
			else setCursorItemIndex = setCursorNodeIndex - list->rangeBegin();

			if (setCursorItemIndex == 0)
				return (list->style()->itemsStyle().isHorizontal()
						? SetCursorEvent::CursorLeftOf : SetCursorEvent::CursorAboveOf);
			else
				return (list->style()->itemsStyle().isHorizontal()
						? SetCursorEvent::CursorRightOf : SetCursorEvent::CursorBelowOf);
		}
	});
}

void HList::removeNodeAndSetCursor(Visualization::VList* list, int removeAtNodeIndex, bool setCursorDown,
				SetCursorEvent::CursorPlacement howToSelectItem)
{
	auto node = list->node();
	Q_ASSERT(removeAtNodeIndex >= 0 && removeAtNodeIndex < node->size());

	// Do not remove nodes which are not visible in this list.
	if (removeAtNodeIndex < list->rangeBegin() || removeAtNodeIndex >= list->rangeEnd()) return;

	node->beginModification("Remove list item");
	node->remove(removeAtNodeIndex);
	node->endModification();
	list->setUpdateNeeded(Visualization::Item::StandardUpdate);

	if (!setCursorDown) --removeAtNodeIndex;
	if (removeAtNodeIndex >=0 && removeAtNodeIndex < node->size())
		scheduleSetCursor(list, node->at<Model::Node>(removeAtNodeIndex), howToSelectItem);
	else
		scheduleSetCursor(list, removeAtNodeIndex < 0 ? 0 : removeAtNodeIndex);
}

void HList::removeNodeAndSetCursor(Visualization::VList* list, int removeAtNodeIndex)
{
	auto node = list->node();
	Q_ASSERT(removeAtNodeIndex >= 0 && removeAtNodeIndex < node->size());

	// Do not remove nodes which are not visible in this list.
	if (removeAtNodeIndex < list->rangeBegin() || removeAtNodeIndex >= list->rangeEnd()) return;

	node->beginModification("Remove list item");
	node->remove(removeAtNodeIndex);
	node->endModification();
	list->setUpdateNeeded(Visualization::Item::StandardUpdate);

	scheduleSetCursor(list, removeAtNodeIndex);
}

void HList::removeNodeAndSetCursor(Visualization::VList* list, Model::Node* removeNode, bool setCursorDown,
			SetCursorEvent::CursorPlacement howToSelectItem)
{
	removeNodeAndSetCursor(list, list->node()->indexOf(removeNode), setCursorDown, howToSelectItem);
}

void HList::removeNodeAndSetCursor(Visualization::VList* list, Model::Node* removeNode)
{
	removeNodeAndSetCursor(list, list->node()->indexOf(removeNode));
}

}
