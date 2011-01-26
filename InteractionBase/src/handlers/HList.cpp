/***********************************************************************************************************************
 * HList.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HList.h"

#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/Scene.h"
#include "FilePersistence/headers/SystemClipboard.h"
#include "ModelBase/headers/nodes/List.h"

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

	if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)  && event->key() == Qt::Key_V)
	{
		FilePersistence::SystemClipboard clipboard;
		int selIndex = list->focusedElementIndex();

		if (clipboard.numNodes() > 0 && selIndex >= 0)
		{
			Model::List* l = dynamic_cast<Model::List*> (list->getNode());
			FilePersistence::SystemClipboard clipboard;
			l->model()->beginModification(l,"paste");
			if (l) l->paste(clipboard, selIndex+1);
			l->model()->endModification();
			target->setUpdateNeeded();
		}

		processed = true;
	}
	else if (event->modifiers() == 0)
	{
		Visualization::Item* toFocus = NULL;
		int currentFocusIndex = list->focusedElementIndex();
		bool first = currentFocusIndex == 0;
		bool last = currentFocusIndex == list->length()-1;

		switch( event->key() )
		{
			case Qt::Key_Up:
			{
				if (list->style()->itemsStyle().direction() == Visualization::LayoutStyle::TopToBottom && !first)
				{
					toFocus = list->at<Visualization::Item>(currentFocusIndex - 1);
					GenericHandler::setFocusDirection(GenericHandler::FROM_BOTTOM);
				}
				if (list->style()->itemsStyle().direction() == Visualization::LayoutStyle::BottomToTop && !last)
				{
					toFocus = list->at<Visualization::Item>(currentFocusIndex + 1);
					GenericHandler::setFocusDirection(GenericHandler::FROM_BOTTOM);
				}
			}
			break;
			case Qt::Key_Down:
			{
				if (list->style()->itemsStyle().direction() == Visualization::LayoutStyle::TopToBottom && !last)
				{
					toFocus = list->at<Visualization::Item>(currentFocusIndex + 1);
					GenericHandler::setFocusDirection(GenericHandler::FROM_TOP);
				}
				if (list->style()->itemsStyle().direction() == Visualization::LayoutStyle::BottomToTop && !first)
				{
					toFocus = list->at<Visualization::Item>(currentFocusIndex - 1);
					GenericHandler::setFocusDirection(GenericHandler::FROM_TOP);
				}
			}
			break;
			case Qt::Key_Left:
			{
				if (list->style()->itemsStyle().direction() == Visualization::LayoutStyle::LeftToRight && !first)
				{
					toFocus = list->at<Visualization::Item>(currentFocusIndex - 1);
					GenericHandler::setFocusDirection(GenericHandler::FROM_RIGHT);
				}
				if (list->style()->itemsStyle().direction() == Visualization::LayoutStyle::RightToLeft && !last)
				{
					toFocus = list->at<Visualization::Item>(currentFocusIndex + 1);
					GenericHandler::setFocusDirection(GenericHandler::FROM_RIGHT);
				}
			}
			break;
			case Qt::Key_Right:
			{
				if (list->style()->itemsStyle().direction() == Visualization::LayoutStyle::LeftToRight && !last)
				{
					toFocus = list->at<Visualization::Item>(currentFocusIndex + 1);
					GenericHandler::setFocusDirection(GenericHandler::FROM_LEFT);
				}
				if (list->style()->itemsStyle().direction() == Visualization::LayoutStyle::RightToLeft && !first)
				{
					toFocus = list->at<Visualization::Item>(currentFocusIndex - 1);
					GenericHandler::setFocusDirection(GenericHandler::FROM_LEFT);
				}
			}
			break;
		}

		processed = toFocus;
		if (toFocus)
		{
			target->scene()->clearSelection();
			toFocus->setSelected(true);
			toFocus->setFocus();
			toFocus->setUpdateNeeded();
			target->setUpdateNeeded();
		}
	}

	if (!processed)GenericHandler::keyPressEvent(target, event);
}

void HList::focusInEvent(Visualization::Item *target, QFocusEvent *event)
{
	Visualization::VList* list = static_cast<Visualization::VList*> (target);
	if (list->length() > 0)
	{
		Visualization::Item* first = list->at<Visualization::Item>(0);
		Visualization::Item* last = list->at<Visualization::Item>(list->length()-1);
		Visualization::Item* toFocus = NULL;

		GenericHandler::FocusDirection dir = GenericHandler::focusDirection();

		// Here we choose which child to focus.
		if (dir == GenericHandler::FROM_LEFT && list->style()->itemsStyle().direction() == Visualization::LayoutStyle::LeftToRight)
			toFocus = first;
		if (dir == GenericHandler::FROM_LEFT && list->style()->itemsStyle().direction() == Visualization::LayoutStyle::RightToLeft)
			toFocus = last;
		if (dir == GenericHandler::FROM_RIGHT && list->style()->itemsStyle().direction() == Visualization::LayoutStyle::LeftToRight)
			toFocus = last;
		if (dir == GenericHandler::FROM_RIGHT && list->style()->itemsStyle().direction() == Visualization::LayoutStyle::RightToLeft)
			toFocus = first;
		if (dir == GenericHandler::FROM_TOP && list->style()->itemsStyle().direction() == Visualization::LayoutStyle::TopToBottom)
			toFocus = first;
		if (dir == GenericHandler::FROM_TOP && list->style()->itemsStyle().direction() == Visualization::LayoutStyle::BottomToTop)
			toFocus = last;
		if (dir == GenericHandler::FROM_BOTTOM && list->style()->itemsStyle().direction() == Visualization::LayoutStyle::TopToBottom)
			toFocus = last;
		if (dir == GenericHandler::FROM_BOTTOM && list->style()->itemsStyle().direction() == Visualization::LayoutStyle::BottomToTop)
			toFocus = first;

		if (!toFocus)
		{
			if ( list->style()->itemsStyle().direction() == Visualization::LayoutStyle::LeftToRight ||
					 list->style()->itemsStyle().direction() == Visualization::LayoutStyle::TopToBottom) toFocus = first;
			else toFocus = last;
		}

		target->scene()->clearSelection();
		toFocus->setSelected(true);
		toFocus->setFocus();
	}

	GenericHandler::focusInEvent(target, event);
}

}
