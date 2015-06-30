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
#include "SelectionAtCursorItem.h"

#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/declarative/DynamicGridFormElement.h"
#include "VisualizationBase/src/declarative/GridLayouter.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/cursor/TextCursor.h"
#include "QGraphicsColorizeEffect"
#include "VisualizationBase/src/items/Item.h"
#include "nodes/ViewSwitcherNode.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(SelectionAtCursorItem, "item")

SelectionAtCursorItem::SelectionAtCursorItem(QVector<Model::Node*> selectableNodes,
											 Visualization::Item* target, StyleType* style, int nrOfColumns)
	: Super(nullptr, style), target_(target), nrOfColumns_(nrOfColumns)
{
	currentNodes_ = arrange(selectableNodes);
	mousePosition_ = target->scene()->lastMouseHoverPosition();
	selectedEffect_ = new QGraphicsColorizeEffect();
	if (hasTextField())
	{
		textField_ = new Visualization::Text(this, &style->nameField(), "Enter name here");
		textField_->setEditable(true);
	}
}

SelectionAtCursorItem::~SelectionAtCursorItem()
{
	//Move the cursor to original owner
	target_->moveCursor();
	//Shouldn't be destroyed
	target_ = nullptr;
	//Destroyed by DeclarativeItem
	focusedItem_ = nullptr;
	SAFE_DELETE_ITEM(textField_);
	//Destroyed when the focused item is destroyed
	if (!focusedItem() || focusedItem()->graphicsEffect() != selectedEffect_)
		SAFE_DELETE(selectedEffect_);
	selectedEffect_ = nullptr;
}

void SelectionAtCursorItem::initializeForms()
{
	auto nodeGrid = (new Visualization::DynamicGridFormElement())
			->setSpacing(5, 5)
			->setMajorAxis(Visualization::GridLayouter::ColumnMajor)
			->setNodesGetter([](Visualization::Item* v)
				{ auto self = static_cast<I*>(v);
				  return self->currentNodes(); });
	//First form: only the nodes
	addForm(nodeGrid);
	//Second form: the nodes and a text field
	addForm((new Visualization::GridLayoutFormElement())
			->setSpacing(5, 5)->setMargins(10)
			->put(0, 0, item<Visualization::Text>(&I::textField_,
									[](I* v) -> const Visualization::TextStyle* { return &v->style()->nameField(); }))
									//[](I* v) { return v->style()->nameField(); }))
			->put(0, 1, nodeGrid));
}

int SelectionAtCursorItem::determineForm()
{
	return hasTextField() ? 1 : 0;
}

bool SelectionAtCursorItem::hasTextField() const
{
	return true;
}

void SelectionAtCursorItem::selectItem(Visualization::Item *item)
{
	if (focusedItem() && focusedItem() != textField())
	{
		focusedItem()->setGraphicsEffect(nullptr);
		selectedEffect_ = new QGraphicsColorizeEffect();
	}
	if (item && item != textField())
		item->setGraphicsEffect(selectedEffect_);
	if (item == textField())
	{
		item->moveCursor();
		item->correspondingSceneCursor<Visualization::TextCursor>()->selectAll();
	}
	else moveCursor(Visualization::Item::MoveOnPosition, QPoint(0, 0));
	focusedItem_ = item;
}

bool SelectionAtCursorItem::executeFocused()
{
	if (hasTextField() && focusedItem() == textField())
		return onSelectText(textField()->text());
	else if (focusedItem())
		return onSelectNode(focusedItem()->node());
	return false;
}

QPoint SelectionAtCursorItem::indexOf(Model::Node *node) const
{
	for (int col = 0; col < currentNodes_.size(); col++)
		for (int row = 0; row < currentNodes_[col].size(); row++)
			if (currentNodes_[col][row] == node)
				return QPoint(col, row);
	return QPoint(-1, -1);
}

QPoint SelectionAtCursorItem::correctCoordinates(QPoint point) const
{
	QPoint result = point;
	if (point.x() < 0)
		result.setX(currentNodes_.size() - 1);
	if (point.x() >= currentNodes_.size())
		result.setX(0);
	if (point.y() < 0)
		result.setY(currentNodes_[point.x()].size() - 1);
	if (point.y() >= currentNodes_[result.x()].size())
		result.setY(0);
	return result;
}

void SelectionAtCursorItem::determineChildren()
{
	Super::determineChildren();
	Item* defaultProxy{};
	if (hasTextField())
		setDefaultMoveCursorProxy(defaultProxy = textField_);
	else
		setDefaultMoveCursorProxy(defaultProxy = findVisualizationOf(currentNodes()[0][0]));
	//TODO@cyril doing this in show doesn't work -> the grid does not have items yet
	if (!focusedItem_)
		selectItem(defaultProxy);
}

void SelectionAtCursorItem::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);
	setPos(mousePosition_.x() - widthInScene() / 2, mousePosition_.y() - heightInScene() / 2);
}

bool SelectionAtCursorItem::sceneEventFilter(QGraphicsItem* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		auto keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up
			|| keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right)
		{
			event->accept();
			//If currently the name is selected, and down key is pressed -> select the item below
			if (focusedItem() == textField() && keyEvent->key() == Qt::Key_Down)
				selectItem(findVisualizationOf(currentNodes()[0][0]));
			//If an item in the top row is selected, and up key is pressed -> select the name
			else if (focusedItem() != textField() && indexOf(focusedItem()->node()).y() == 0
					 && keyEvent->key() == Qt::Key_Up && hasTextField())
				selectItem(textField());
			//Otherwise just switch between the normal items
			else if (focusedItem() != textField() || !hasTextField())
			{
				auto pos = indexOf(focusedItem()->node());
				if (keyEvent->key() == Qt::Key_Down)
					pos.setY(pos.y() + 1);
				else if (keyEvent->key() == Qt::Key_Up)
					pos.setY(pos.y() - 1);
				else if (keyEvent->key() == Qt::Key_Left)
					pos.setX(pos.x() - 1);
				else
					pos.setX(pos.x() + 1);
				pos = correctCoordinates(pos);
				selectItem(findVisualizationOf(currentNodes()[pos.x()][pos.y()]));
			}
			return true;
		}
		else if (keyEvent->key() == Qt::Key_Return)
		{
			event->accept();
			if (executeFocused())
			{
				hideSelection();
				return true;
			}
		}
	}
	else if (event->type() == QEvent::GraphicsSceneMousePress)
	{
		auto asItem = Visualization::Item::envisionItem(watched);
		asItem = asItem ? asItem->findAncestorWithNode() : asItem;
		if (asItem && isAncestorOf(watched))
		{
			selectItem(asItem);
			if (executeFocused())
			{
				hideSelection();
				return true;
			}
		}
	}
	return false;
}

QVector<QVector<Model::Node*>> SelectionAtCursorItem::arrange(QVector<Model::Node*> nodes)
{
	QVector<QVector<Model::Node*>> result;
	result.resize(std::min(nrOfColumns_, nodes.size()));
	for (int index = 0; index < nodes.size(); index++)
	{
		int column = index % nrOfColumns_;
		result[column].append(nodes[index]);
	}
	return result;
}

}
