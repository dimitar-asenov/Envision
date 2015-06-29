/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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

#include "HSelectionAtCursorItem.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/VInfoNode.h"
#include "vis/SelectionAtCursorItem.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/cursor/TextCursor.h"

namespace Interaction {

HSelectionAtCursorItem* HSelectionAtCursorItem::instance()
{
	static HSelectionAtCursorItem h;
	return &h;
}

void HSelectionAtCursorItem::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto asSelection = static_cast<SelectionAtCursorItem*>(target);
	//When pressing the return key, execute the selected element
	if (event->key() == Qt::Key_Return)
	{
		event->accept();
		asSelection->executeFocused();
		SelectionAtCursorItem::hide();
	}
	else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up
			 || event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
	{
		event->accept();
		auto current = asSelection->focusedItem();
		//If currently the name is selected, and down key is pressed -> select the item below
		if (current == asSelection->textField() && event->key() == Qt::Key_Down)
			asSelection->selectItem(asSelection->findVisualizationOf(asSelection->currentNodes()[0][0]));
		//If an item in the top row is selected, and up key is pressed -> select the name
		else if (current != asSelection->textField() && asSelection->indexOf(current->node()).y() == 0
				 && event->key() == Qt::Key_Up && asSelection->hasTextField())
			asSelection->selectItem(asSelection->textField());
		//Otherwise just switch between the normal items
		else if (current != asSelection->textField() || !asSelection->hasTextField())
		{
			auto pos = asSelection->indexOf(current->node());
			if (event->key() == Qt::Key_Down)
				pos.setY(pos.y() + 1);
			else if (event->key() == Qt::Key_Up)
				pos.setY(pos.y() - 1);
			else if (event->key() == Qt::Key_Left)
				pos.setX(pos.x() - 1);
			else
				pos.setX(pos.x() + 1);
			pos = asSelection->correctCoordinates(pos);
			asSelection->selectItem(asSelection->findVisualizationOf(asSelection->currentNodes()[pos.x()][pos.y()]));
		}
	}
	else GenericHandler::keyPressEvent(target, event);
}

void HSelectionAtCursorItem::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	//Clicking on an item should select a view to open
	auto asSelection = static_cast<SelectionAtCursorItem*>(target);
	//Get the item which was actually clicked on
	auto actuallyPressed = target->scene()->itemAt(event->scenePos(), QTransform());
	auto item = Visualization::Item::envisionItem(actuallyPressed);
	while (item && !item->hasNode())
		item = item->parent();
	//Execute the clicked element
	if (item)
	{
		asSelection->selectItem(item);
		asSelection->executeFocused();
		SelectionAtCursorItem::hide();
	}
}

}
