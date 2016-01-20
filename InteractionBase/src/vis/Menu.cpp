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
#include "Menu.h"

#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/declarative/DynamicGridFormElement.h"
#include "VisualizationBase/src/items/Item.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(Menu, "item")

Menu* Menu::instance{};

Menu::Menu(QVector<QVector<Visualization::Item*> > items, Visualization::Item* selectedItem,
			Visualization::Item *target, StyleType *style)
	: Super{nullptr, style}, target_{target}, currentItems_{items}
{
	mousePosition_ = target->scene()->lastMouseHoverPosition();
	setFiltersChildEvents(true);

	// Select the middle item
	QApplication::postEvent(target_->scene(),
		new Visualization::CustomSceneEvent{ [this, selectedItem]()
			{
				selectItem(selectedItem);
			}});
}

Menu::~Menu()
{
	//Move the cursor to original owner
	target_->moveCursor();
	//Shouldn't be destroyed
	target_ = nullptr;
	//Destroyed by DeclarativeItem
	focusedItem_ = nullptr;
}

void Menu::hide()
{
	if (instance)
		QApplication::postEvent(instance->scene(),
								new Visualization::CustomSceneEvent{ [&]() { hideNow(); }});
}

void Menu::hideNow()
{
	SAFE_DELETE_ITEM(instance);
	instance = nullptr;
}

void Menu::initializeForms()
{
	auto nodeGrid = (new Visualization::DynamicGridFormElement{})
			->setSpacing(5, 5)
			->setMajorAxis(Visualization::GridLayouter::ColumnMajor)
			->setItemsGetter([](Visualization::Item* v)
				{ auto self = static_cast<I*>(v);
				  return self->currentItems(); });
	//Just have a grid of nodes
	addForm(nodeGrid);
}

void Menu::selectItem(Visualization::Item* item)
{
	//Make sure the item is part of the menu
	if (indexOf(item).x() != - 1)
	{
		if (focusedItem())
			focusedItem()->setGraphicsEffect(nullptr);
		if (item)
			item->setGraphicsEffect(new QGraphicsColorizeEffect{});

		item->moveCursor();
		focusedItem_ = item;
	}
}

bool Menu::executeFocused()
{
	if (focusedItem())
		return executeEntry(focusedItem());
	return false;
}

QPoint Menu::indexOf(Visualization::Item *item) const
{
	for (int col = 0; col < currentItems_.size(); col++)
		for (int row = 0; row < currentItems_[col].size(); row++)
			if (currentItems_[col][row] == item)
				return QPoint(col, row);
	return QPoint(-1, -1);
}

QPoint Menu::correctCoordinates(QPoint point) const
{
	QPoint result = point;
	if (point.x() < 0)
		result.setX(currentItems_.size() - 1);
	if (point.x() >= currentItems_.size())
		result.setX(0);
	if (point.y() < 0)
		result.setY(currentItems_[point.x()].size() - 1);
	if (point.y() >= currentItems_[result.x()].size())
		result.setY(0);
	return result;
}

void Menu::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);
	setPos(mousePosition_.x() - widthInScene() / 2, mousePosition_.y() - heightInScene() / 2);
}

bool Menu::sceneEventFilter(QGraphicsItem* watched, QEvent* event)
{
	//If we are in focus mode, and not F2 or Return is pressed, we don't handle any events here.
	if (inFocusMode_)
	{
		bool isFocusModeKey = false;
		if (event->type() == QEvent::KeyPress)
			if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_F2
					|| static_cast<QKeyEvent*>(event)->key() == Qt::Key_Return
					|| static_cast<QKeyEvent*>(event)->key() == Qt::Key_Enter)
				isFocusModeKey = true;
		if (!isFocusModeKey) return false;
	}
	if (event->type() == QEvent::KeyPress)
	{
		auto keyEvent = static_cast<QKeyEvent*>(event);
		//Only switch between when in focus mode
		if ((keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up
			|| keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right)
			&& !inFocusMode_)
		{
			event->accept();
			//Switch between the normal items depending on the input key direction
			auto pos = indexOf(focusedItem());
			if (keyEvent->key() == Qt::Key_Down)
				pos.setY(pos.y() + 1);
			else if (keyEvent->key() == Qt::Key_Up)
				pos.setY(pos.y() - 1);
			else if (keyEvent->key() == Qt::Key_Left)
				pos.setX(pos.x() - 1);
			else
				pos.setX(pos.x() + 1);
			pos = correctCoordinates(pos);
			selectItem(currentItems()[pos.x()][pos.y()]);
			return true;
		}
		else if (keyEvent->key() == Qt::Key_F2 || (inFocusMode_
					&& (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)))
		{
			inFocusMode_ = !inFocusMode_;
			if (inFocusMode_) startFocusMode(focusedItem_);
			else
			{
				endFocusMode(focusedItem_);
				selectItem(focusedItem_);
			}
			return true;
		}
		else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
		{
			if (executeFocused())
			{
				hide();
				return true;
			}
		}
	}
	else if (event->type() == QEvent::GraphicsSceneMousePress)
	{
		auto asItem = Visualization::Item::envisionItem(watched);
		while (asItem && indexOf(asItem).x() == -1)
			asItem = asItem->parent();
		if (asItem)
		{
			selectItem(asItem);
			if (executeFocused())
			{
				hide();
				return true;
			}
		}
	}
	return false;
}

}
