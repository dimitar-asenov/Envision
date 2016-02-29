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

#include "VList.h"
#include "Static.h"
#include "../declarative/DeclarativeItem.hpp"
#include "../shapes/Shape.h"

namespace Visualization {

DEFINE_ITEM_COMMON(VList, "item")

VList::VList(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{}

int VList::indexOfChildOrSubChild(const Item* item) const
{
	if (!item) return -1;
	auto currentItem = item;
	auto parent = item->parent();
	while (parent)
	{
		if (parent == this) break;
		currentItem = parent;
		parent = parent->parent();
	}
	if (!parent) return -1;
	for (int i = 0; i < length(); ++i)
		if (currentItem == itemAt<Item>(i)) return i;
	return -1;
}

void VList::determineRange()
{
	setRange(0, node()->size());
}

void VList::setRange(int begin, int end)
{
	Q_ASSERT(0 <= begin);
	Q_ASSERT(begin <= end);
	Q_ASSERT(end <= node()->size());
	Q_ASSERT(begin < node()->size() || node()->size() == 0);

	rangeBegin_ = begin;
	rangeEnd_ = end;
}

void VList::initializeForms()
{
	auto listOfNodesGetter = [](Item* i)
							{
								auto v = static_cast<VList*>(i);
								return v->node()->nodes().toList().mid(v->rangeBegin_, v->rangeEnd_ - v->rangeBegin_);
							};
	auto spaceBetweenElementsGetter = [](Item* i)
							{return static_cast<VList*>(i)->style()->itemsStyle().spaceBetweenElements();};
	auto hasCursorWhenEmptyGetter = [](Item* i)
							{return static_cast<VList*>(i)->style()->itemsStyle().hasCursorWhenEmpty();};
	auto notLocationEquivalentGetter = [](Item* i)
							{return static_cast<VList*>(i)->style()->itemsStyle().notLocationEquivalentCursors();};
	auto noBoundaryCursorsGetter = [](Item* i)
							{return static_cast<VList*>(i)->style()->itemsStyle().noBoundaryCursorsInsideShape();};
	auto noInnerCursorsGetter = [](Item* i)
							{return static_cast<VList*>(i)->style()->itemsStyle().noInnerCursors();};
	auto minWidthGetter = [](Item* i)
							{return static_cast<VList*>(i)->style()->minWidth();};
	auto minHeightGetter = [](Item* i)
							{return static_cast<VList*>(i)->style()->minHeight();};

	// Form 0: horizontal orientation
	addForm((new SequentialLayoutFormElement{})
			->setHorizontal()->setSpaceBetweenElements(spaceBetweenElementsGetter)
			->setHasCursorWhenEmpty(hasCursorWhenEmptyGetter)
			->setNotLocationEquivalentCursors(notLocationEquivalentGetter)
			->setNoBoundaryCursors(noBoundaryCursorsGetter)
			->setNoInnerCursors(noInnerCursorsGetter)
			->setListOfNodes(listOfNodesGetter)
			->setMinWidthGetter(minWidthGetter)
			->setMinHeightGetter(minHeightGetter));
	// Form 1: vertical orientation
	addForm((new SequentialLayoutFormElement{})
			->setVertical()->setSpaceBetweenElements(spaceBetweenElementsGetter)
			->setHasCursorWhenEmpty(hasCursorWhenEmptyGetter)
			->setNotLocationEquivalentCursors(notLocationEquivalentGetter)
			->setNoBoundaryCursors(noBoundaryCursorsGetter)
			->setNoInnerCursors(noInnerCursorsGetter)
			->setListOfNodes(listOfNodesGetter)
			->setMinWidthGetter(minWidthGetter)
			->setMinHeightGetter(minHeightGetter));
	// Form 2: EmptyList with a tip
	addForm(item<Static>(&I::emptyTip_, [](I* v){ return &v->style()->selectedTip(); }));
}

int VList::determineForm()
{
	determineRange();

	bool showTip = itemOrChildHasFocus() && node()->isEmpty() && style()->showTipWhenSelectedAndEmpty();
	if (!showTip && node()->isEmpty() && !style()->drawShapeWhenEmpty() ) removeShape();
	else useShape();


	if (showTip) return 2;

	if (style()->itemsStyle().isHorizontal()) return 0;
	else return 1;
}

bool VList::moveCursor(CursorMoveDirection dir, QPoint reference, CursorMoveOptions options)
{
	bool startsFocused = hasFocus();

	// If we're already focused and the user pressed a keyboard key, do not stay within the item
	if (startsFocused && node()->isEmpty() && style()->showTipWhenSelectedAndEmpty() && dir != MoveOnPosition
			&& dir != MoveOnTop && dir != MoveOnLeft && dir != MoveOnBottom && dir != MoveOnRight
			&& dir != MoveOnTopLeft && dir != MoveOnBottomRight && dir != MoveOnCenter)
		return false;

	bool res = Super::moveCursor(dir, reference, options);

	if (res && !startsFocused && node()->isEmpty() && style()->showTipWhenSelectedAndEmpty())
		setUpdateNeeded(StandardUpdate);

	return res;
}

void VList::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);

	if (node()->isEmpty() && style()->showTipWhenSelectedAndEmpty() && (heightInLocal() == 0 || widthInLocal() == 0))
	{
		if (heightInLocal() == 0) setHeight(1);
		if (widthInLocal() == 0) setWidth(1);
	}
}

QList<ItemRegion> VList::regions()
{
	if (!node()->isEmpty() || !style()->showTipWhenSelectedAndEmpty())
		return Super::regions();

	//Otherwise return a whole item region
	auto ir = ItemRegion{boundingRect().toRect()};

	Cursor* cur = new Cursor{this, Cursor::BoxCursor};
	cur->setRegion( ir.region() );
	cur->setPosition( ir.region().center() );
	ir.setCursor(cur);

	return {ir};
}

void VList::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Super::paint(painter, option, widget);

	if (node()->isEmpty() || !style()->useBackgroundColors() || style()->backgroundColors().isEmpty()) return;

	auto children = childItems();
	if (children.isEmpty()) return;

	auto horizontal = style()->itemsStyle().isHorizontal();
	// Sort the children so that they appear in the right order
	if (horizontal)
		qSort(children.begin(), children.end(), [](const Item* left, const Item* right)
		{return left->pos().x() < right->pos().x();});
	else
		qSort(children.begin(), children.end(), [=](const Item* left, const Item* right)
		{return left->pos().y() < right->pos().y();});

	QPoint topLeft{0, 0};
	auto w = widthInLocal();
	auto h = heightInLocal();
	QPoint endPoint{w, h};

	if (auto shape = getShape())
	{
		auto contentRect = shape->contentRect();
		topLeft = contentRect.topLeft();
		w = contentRect.width();
		h = contentRect.height();

		endPoint.setX(topLeft.x() + w);
		endPoint.setY(topLeft.y() + h);
	}


	for (int i = 0; i<children.size(); ++i)
	{
		if (i+1 < children.size())
		{
			if (horizontal)
				w = (children.at(i)->pos().x() + children.at(i)->widthInParent() + children.at(i+1)->pos().x())/2
						- topLeft.x();
			else
				h = (children.at(i)->pos().y() + children.at(i)->heightInParent() + children.at(i+1)->pos().y())/2
						- topLeft.y();
		}
		else
		{
			if (horizontal) w = endPoint.x() - topLeft.x();
			else h = endPoint.y() - topLeft.y();
		}

		painter->fillRect(topLeft.x(), topLeft.y(), w, h,
				style()->backgroundColors()[i % style()->backgroundColors().size()]);

		if (horizontal) topLeft.rx() += w;
		else topLeft.ry() += h;
	}

}

bool VList::ignoresCopyAndPaste()
{
	return suppressDefaultCopyPasteHandler_;
}

}
