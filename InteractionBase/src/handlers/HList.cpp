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

	if (event->matches(QKeySequence::Paste) && list->scene()->mainCursor()->owner() == list)
	{
		processed = true;
		FilePersistence::SystemClipboard clipboard;
		int selIndex = list->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
		if (selIndex > list->XXlength()) selIndex = list->XXlength();
		if (selIndex < 0) selIndex = 0;
		selIndex += list->rangeBegin();

		if (clipboard.numNodes() > 0)
		{
			Model::List* l = dynamic_cast<Model::List*> (list->node());
			FilePersistence::SystemClipboard clipboard;
			l->model()->beginModification(l, "paste into list");
			if (l) l->paste(clipboard, selIndex);
			l->model()->endModification();
			target->setUpdateNeeded(Visualization::Item::StandardUpdate);
		}
	}
	else if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)  && event->key() == Qt::Key_V)
	{
		processed = true;
		FilePersistence::SystemClipboard clipboard;
		int selIndex = list->focusedNodeIndex();

		if (clipboard.numNodes() > 0 && selIndex >= 0)
		{
			Model::List* l = dynamic_cast<Model::List*> (list->node());
			FilePersistence::SystemClipboard clipboard;
			l->model()->beginModification(l, "paste into list");
			if (l) l->paste(clipboard, selIndex+1);
			l->model()->endModification();
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
		if (index >=0 && index < list->XXlength())
			removeNodeAndSetCursor(list, index + list->rangeBegin());
	}

	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
	{
		bool createDown = event->modifiers() == Qt::NoModifier;
		bool createRight = event->modifiers() == Qt::ShiftModifier;

		if ((createDown && !list->style()->itemsStyle().isHorizontal())
				|| ( createRight && list->style()->itemsStyle().isHorizontal()) )
		{
			auto newElem = list->node()->createDefaultElement();
			if (newElem)
			{
				processed = true;

				int index = -1;
				if (list->scene()->mainCursor() && list->scene()->mainCursor()->owner() == list)
					index = list->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
				else
				{
					index = list->focusedItemIndex();
					if (index >= 0) ++index;
				}

				// At this point, index indicates where in the LAYOUT we should put the new element.
				if (index >= 0)
				{
					list->node()->model()->beginModification(list->node(), "add new list element");
					list->node()->insert(index + list->rangeBegin(), newElem);
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

void HList::scheduleSetCursor(Visualization::VList* list, Model::Node* listNodeToSelect,
		SetCursorEvent::CursorPlacement howToSelectItem)
{
	list->scene()->addPostEventAction( new SetCursorEvent(list, listNodeToSelect, howToSelectItem));
}

void HList::scheduleSetCursor(Visualization::VList* list, int setCursorNodeIndex)
{
	Q_ASSERT(setCursorNodeIndex >= 0 && setCursorNodeIndex <= list->node()->size());

	list->scene()->addPostEventAction(new SetCursorEvent(
		[=](){
			int setCursorItemIndex = 0;
			if (setCursorNodeIndex < list->rangeBegin()) setCursorItemIndex = 0;
			else if (setCursorNodeIndex > list->rangeEnd()) setCursorItemIndex = list->XXlength();
			else setCursorItemIndex = setCursorNodeIndex - list->rangeBegin();

			if (setCursorItemIndex == 0) return list->XXlength() ? list->itemAt<Visualization::Item>(0) : list;
			else return list->itemAt<Visualization::Item>(setCursorItemIndex-1);
		},
		[=](){
			int setCursorItemIndex = 0;
			if (setCursorNodeIndex < list->rangeBegin()) setCursorItemIndex = 0;
			else if (setCursorNodeIndex > list->rangeEnd()) setCursorItemIndex = list->XXlength();
			else setCursorItemIndex = setCursorNodeIndex - list->rangeBegin();

			if (setCursorItemIndex == 0)
				return (list->style()->itemsStyle().isHorizontal()
						? SetCursorEvent::CursorLeftOf : SetCursorEvent::CursorAboveOf);
			else
				return (list->style()->itemsStyle().isHorizontal()
						? SetCursorEvent::CursorRightOf : SetCursorEvent::CursorBelowOf);
		}
	));
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
