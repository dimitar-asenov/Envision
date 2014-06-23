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

#include "SequentialLayoutFormElement.h"

#include "DeclarativeItemBase.h"
#include "GridLayouter.h"
#include "../renderer/ModelRenderer.h"
#include "../items/Item.h"
#include "../items/ItemRegion.h"
#include "../cursor/LayoutCursor.h"

#include "ModelBase/src/nodes/List.h"

namespace Visualization {

SequentialLayoutFormElement::SequentialLayoutFormElement(const SequentialLayoutFormElement& other)
:	LayoutFormElement{other}, listNodeGetter_{other.listNodeGetter_}, nodeListGetter_{other.nodeListGetter_},
 	itemListGetter_{other.itemListGetter_}, spaceBetweenElementsGetter_{other.spaceBetweenElementsGetter_},
 	defaultSpaceBetweenElements_{other.defaultSpaceBetweenElements_}, orientation_{other.orientation_},
 	alignment_{other.alignment_}, forward_{other.forward_}, minWidth_{other.minWidth_}, minHeight_{other.minHeight_}
{
}

SequentialLayoutFormElement::~SequentialLayoutFormElement()
{
	for (auto data : itemData_)
	{
		for (auto item : data->items_)
			SAFE_DELETE_ITEM(item);
		SAFE_DELETE(data);
	}
}


SequentialLayoutFormElement* SequentialLayoutFormElement::clone() const
{
	return new SequentialLayoutFormElement(*this);
}

void SequentialLayoutFormElement::computeSize(Item* item, int availableWidth, int availableHeight)
{
	auto& itemData = dataForItem(item);
	auto& itemList = itemData.items_;
	auto& itemPositions = itemData.itemPositionWithinLayout_;
	bool horizontal = orientation_ == Qt::Horizontal;
	int spacing = spaceBetweenElements(item);
	int size = itemList.size();

	QSize finalSize;

	if (forward_)
	{
		if (horizontal)
		{
			finalSize = GridLayouter::computeSize<false>(availableWidth, availableHeight,
				[](){return 1;},	// numRows
				[size](){return size;},	// numColumns
				[](int, int){return true;},	// has
				[](int, int){return QPair<int, int>{1, 1};},	// spanGrid
				[&itemList](int x, int){return itemList[x]->widthInParent();},	// width
				[&itemList](int x, int){return itemList[x]->heightInParent();},	// height
				[](int, int, int, int){},	// computeElementSize
				[&itemList](int x, int, int w, int h){itemList[x]->changeGeometry(w, h);},	// changeGeometry
				[&itemList](int x, int){return itemList[x]->sizeDependsOnParent();},	// isStretchable
				[&itemPositions](int x, int, QPoint pos){itemPositions[x] = pos;},	// setPosition
				[](int){return 0;},	// rowStretchFactors
				[&itemList](int x){return itemList[x]->sizeDependsOnParent() ? 1 : 0;},	// columnStretchFactors
				[](int, int){return LayoutStyle::Alignment::Left;},	// horizontalAlignment
				[this](int, int){return alignment_;},	// verticalAlignment
				[](){return 0;},	// spaceBetweenRows
				[spacing](){return spacing;},	// spaceBetweenColumns
				[this](){return topMargin();},	// topMargin
				[this](){return bottomMargin();},	// bottomMargin
				[this](){return leftMargin();},	// leftMargin
				[this](){return rightMargin();},	// rightMargin
				[this](){return minWidth_;},	// minWidth
				[this](){return minHeight_;}	// minHeight
			);
		}
		else
		{
			finalSize = GridLayouter::computeSize<false>(availableWidth, availableHeight,
				[size](){return size;},	// numRows
				[](){return 1;},	// numColumns
				[](int, int){return true;},	// has
				[](int, int){return QPair<int, int>{1, 1};},	// spanGrid
				[&itemList](int, int y){return itemList[y]->widthInParent();},	// width
				[&itemList](int, int y){return itemList[y]->heightInParent();},	// height
				[](int, int, int, int){},	// computeElementSize
				[&itemList](int, int y, int w, int h){itemList[y]->changeGeometry(w, h);},	// changeGeometry
				[&itemList](int, int y){return itemList[y]->sizeDependsOnParent();},	// isStretchable
				[&itemPositions](int, int y, QPoint pos){itemPositions[y] = pos;},	// setPosition
				[&itemList](int y){return itemList[y]->sizeDependsOnParent() ? 1 : 0;},	// rowStretchFactors
				[](int){return 0;},	// columnStretchFactors
				[this](int, int){return alignment_;},	// horizontalAlignment
				[](int, int){return LayoutStyle::Alignment::Top;},	// verticalAlignment
				[spacing](){return spacing;},	// spaceBetweenRows
				[](){return 0;},	// spaceBetweenColumns
				[this](){return topMargin();},	// topMargin
				[this](){return bottomMargin();},	// bottomMargin
				[this](){return leftMargin();},	// leftMargin
				[this](){return rightMargin();},	// rightMargin
				[this](){return minWidth_;},	// minWidth
				[this](){return minHeight_;}	// minHeight
			);
		}
	}
	else // Reversed order
	{
		auto invert = [size](int i){return size-1-i;};
		if (horizontal)
		{
			finalSize = GridLayouter::computeSize<false>(availableWidth, availableHeight,
				[](){return 1;},	// numRows
				[size](){return size;},	// numColumns
				[](int, int){return true;},	// has
				[](int, int){return QPair<int, int>{1, 1};},	// spanGrid
				[&itemList, invert](int x, int){return itemList[invert(x)]->widthInParent();},	// width
				[&itemList, invert](int x, int){return itemList[invert(x)]->heightInParent();},	// height
				[](int, int, int, int){},	// computeElementSize
				[&itemList, invert](int x, int, int w, int h){itemList[invert(x)]->changeGeometry(w, h);},	// changeGeom.
				[&itemList, invert](int x, int){return itemList[invert(x)]->sizeDependsOnParent();},	// isStretchable
				[&itemPositions, invert](int x, int, QPoint pos){itemPositions[invert(x)] = pos;},	// setPosition
				[](int){return 0;},	// rowStretchFactors
				[&itemList, invert](int x){return itemList[invert(x)]->sizeDependsOnParent() ? 1 : 0;},	// columnStretchF.
				[](int, int){return LayoutStyle::Alignment::Left;},	// horizontalAlignment
				[this](int, int){return alignment_;},	// verticalAlignment
				[](){return 0;},	// spaceBetweenRows
				[spacing](){return spacing;},	// spaceBetweenColumns
				[this](){return topMargin();},	// topMargin
				[this](){return bottomMargin();},	// bottomMargin
				[this](){return leftMargin();},	// leftMargin
				[this](){return rightMargin();},	// rightMargin
				[this](){return minWidth_;},	// minWidth
				[this](){return minHeight_;}	// minHeight
			);
		}
		else
		{
			finalSize = GridLayouter::computeSize<false>(availableWidth, availableHeight,
				[size](){return size;},	// numRows
				[](){return 1;},	// numColumns
				[](int, int){return true;},	// has
				[](int, int){return QPair<int, int>{1, 1};},	// spanGrid
				[&itemList, invert](int, int y){return itemList[invert(y)]->widthInParent();},	// width
				[&itemList, invert](int, int y){return itemList[invert(y)]->heightInParent();},	// height
				[](int, int, int, int){},	// computeElementSize
				[&itemList, invert](int, int y, int w, int h){itemList[invert(y)]->changeGeometry(w, h);},	// changeGeom.
				[&itemList, invert](int, int y){return itemList[invert(y)]->sizeDependsOnParent();},	// isStretchable
				[&itemPositions, invert](int, int y, QPoint pos){itemPositions[invert(y)] = pos;},	// setPosition
				[&itemList, invert](int y){return itemList[invert(y)]->sizeDependsOnParent() ? 1 : 0;},	// rowStretchF.
				[](int){return 0;},	// columnStretchFactors
				[this](int, int){return alignment_;},	// horizontalAlignment
				[](int, int){return LayoutStyle::Alignment::Top;},	// verticalAlignment
				[spacing](){return spacing;},	// spaceBetweenRows
				[](){return 0;},	// spaceBetweenColumns
				[this](){return topMargin();},	// topMargin
				[this](){return bottomMargin();},	// bottomMargin
				[this](){return leftMargin();},	// leftMargin
				[this](){return rightMargin();},	// rightMargin
				[this](){return minWidth_;},	// minWidth
				[this](){return minHeight_;}	// minHeight
			);
		}
	}

	setSize(item, finalSize);
}

void SequentialLayoutFormElement::setItemPositions(Item* item, int parentX, int parentY)
{
	auto& data = dataForItem(item);
	for (int i = 0; i<data.items_.size(); ++i)
	{
		QPointF newPos{parentX + x(item) + data.itemPositionWithinLayout_[i].x() + leftMargin(),
					parentY + y(item) + data.itemPositionWithinLayout_[i].y() + topMargin()};

		if (newPos != data.items_[i]->pos())
			data.items_[i]->setPos(newPos); // setting the position is an expensive operation so only do it if it changed
	}
}

void SequentialLayoutFormElement::synchronizeWithItem(Item* item)
{
	if (listNodeGetter_)
	{
		Model::List* listNode = listNodeGetter_(item);
		synchronizeWithNodes(item, listNode->children());
	}
	else if (nodeListGetter_)
		synchronizeWithNodes(item, nodeListGetter_(item));
	else
	{
		Q_ASSERT(itemListGetter_);
		synchronizeWithItems(item, itemListGetter_(item));
	}

	auto& itemData = dataForItem(item);
	itemData.itemPositionWithinLayout_.resize(itemData.items_.size());
	//Note that we do not need to initialize the positions in any way since they will be overwritten in computeSize()

	for (auto i : itemData.items_) i->setParentItem(item);
}

bool SequentialLayoutFormElement::sizeDependsOnParent(const Item* item) const
{
	for (auto i : dataForItem(item).items_)
		if (i->sizeDependsOnParent())
			return true;
	return false;
}

void SequentialLayoutFormElement::destroyChildItems(Item* item,
		QList<const Item* const DeclarativeItemBase::*> handledChildren)
{
	LayoutFormElement::destroyChildItems(item, handledChildren);

	auto dataIterator = itemData_.find(item);
	if (dataIterator != itemData_.end())
	{
		for (auto item : dataIterator.value()->items_)
			SAFE_DELETE_ITEM(item);

		SAFE_DELETE(dataIterator.value());
		itemData_.erase(dataIterator);
	}
}

SequentialLayoutFormElement::ItemData& SequentialLayoutFormElement::dataForItem(const Item* item) const
{
	auto dataIterator = itemData_.find(item);
	if (dataIterator != itemData_.end()) return *(dataIterator.value());

	auto data = new ItemData;
	itemData_.insert(item, data);
	return *data;
}

int SequentialLayoutFormElement::spaceBetweenElements(Item* item)
{
	if (spaceBetweenElementsGetter_)
		return spaceBetweenElementsGetter_(item);
	return defaultSpaceBetweenElements_;
}

void SequentialLayoutFormElement::synchronizeWithNodes(Item* item, const QList<Model::Node*>& nodes)
{
	item->synchronizeCollections(nodes, dataForItem(item).items_,
		[](Model::Node* node, Item* item){return item->node() == node;},
		[](Item* parent, Model::Node* node){return parent->renderer()->render(parent, node);},
		[](Item* parent, Model::Node* node, Item*& item){return parent->renderer()->sync(item, parent, node);});
}

void SequentialLayoutFormElement::synchronizeWithItems(Item* item, const QList<Item*>& items)
{
	item->synchronizeCollections(items, dataForItem(item).items_,
		[](Item* newItem, Item* oldItem){return newItem == oldItem;},
		[](Item*, Item* newItem){return newItem;},
		[](Item*, Item*, Item*&){return false;});
}

QList<ItemRegion> SequentialLayoutFormElement::regions(DeclarativeItemBase* item, int parentX, int parentY)
{
	QList<ItemRegion> regs;
	auto& itemList = dataForItem(item).items_;

	// If this layout is not visible return no regions
	if (itemList.isEmpty() && !hasCursorWhenEmpty(item))
		return regs;

	bool extraCursors = false;
	if (parent() == nullptr)
	{
		// This is a top-level list. Take into consideration the style of the parent item and its shape.
		extraCursors = item->hasShape() && item->style()->extraCursorsOutsideShape();
	}

	bool horizontal = (orientation_ == Qt::Horizontal);

	auto elementBoundary = QRect(QPoint(x(item) + parentX, y(item) + parentY), size(item));
	if (extraCursors && itemList.isEmpty())
	{
		if (horizontal)
		{
			elementBoundary.setTop(0);
			elementBoundary.setHeight( item->heightInLocal() );
		}
		else
		{
			elementBoundary.setLeft(0);
			elementBoundary.setWidth( item->widthInLocal() );
		}
	}

	auto wholeArea = extraCursors ? QRect(QPoint(0, 0), item->sizeInLocal().toSize()) : elementBoundary;

	auto elementsArea = elementBoundary.adjusted(+leftMargin(), topMargin(), -rightMargin(), -bottomMargin());

	// This is the rectangle half-way between the bounding box of the layout and elementsArea.
	QRect midArea = QRect( (wholeArea.left() + elementsArea.left()) / 2, (wholeArea.top() + elementsArea.top()) / 2,
			(wholeArea.width() + elementsArea.width()) / 2, (wholeArea.height() + elementsArea.height()) / 2 );

	//int offset = (spaceBetweenElements(item) > 0) ? spaceBetweenElements(item)/2 : 1;
	int offset = 1;

	int last = forward_ ?
			( horizontal ? midArea.left() : midArea.top()) :
			( horizontal ? midArea.right() + offset : midArea.bottom() + offset);

	for (int i = 0; i<itemList.size(); ++i)
	{
		ItemRegion cursorRegion;
		ItemRegion itemRegion;
		if (horizontal && forward_)
		{
			cursorRegion.setRegion(QRect(last, elementBoundary.top(), itemList[i]->x() - last, elementBoundary.height()));
			itemRegion.setRegion(QRect(itemList[i]->x(), elementBoundary.top(), itemList[i]->widthInParent(),
												elementBoundary.height()));
			last = itemList[i]->xEndInParent() + offset;
		}
		else if (horizontal && !forward_)
		{
			cursorRegion.setRegion(QRect(itemList[i]->xEndInParent()+1, elementBoundary.top(), last,
												  elementBoundary.height()));
			itemRegion.setRegion(QRect(itemList[i]->x(), elementBoundary.top(), itemList[i]->widthInParent(),
												elementBoundary.height()));
			last = itemList[i]->x() - offset;
		}
		else if (!horizontal && forward_)
		{
			cursorRegion.setRegion(QRect(elementBoundary.left(), last,  elementBoundary.width(), itemList[i]->y() - last));
			itemRegion.setRegion(QRect(elementBoundary.left(), itemList[i]->y(), elementBoundary.width(),
												itemList[i]->heightInParent()));
			last = itemList[i]->yEndInParent() + offset;
		}
		else
		{
			cursorRegion.setRegion(QRect(elementBoundary.left(), itemList[i]->yEndInParent()+1, elementBoundary.width(),
												  last));
			itemRegion.setRegion(QRect(elementBoundary.left(), itemList[i]->y(), elementBoundary.width(),
												itemList[i]->heightInParent()));
			last = itemList[i]->y() - offset;
		}

		itemRegion.setItem(itemList[i]);
		adjustCursorRegionToAvoidZeroSize(cursorRegion.region(), horizontal, !extraCursors && i==0, false);

		// Note below, that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(item, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		lc->setOwnerElement(this);
		cursorRegion.setCursor(lc);
		lc->setIndex(i);
		if (horizontal)
		{
			lc->setVisualizationPosition(QPoint(cursorRegion.region().center().x(), cursorRegion.region().top()));
			lc->setVisualizationSize(QSize(2, elementBoundary.height()));
		}
		else
		{
			lc->setVisualizationPosition(QPoint(cursorRegion.region().left(), cursorRegion.region().center().y()));
			lc->setVisualizationSize(QSize(elementBoundary.width(), 2));
		}
		lc->setOwnerElement(this);
		if (i==0 && !extraCursors) lc->setIsAtBoundary(true);

		cursorRegion.cursor()->setRegion(cursorRegion.region());
		if (notLocationEquivalentCursors(item)) lc->setNotLocationEquivalent(true);

		// Skip cursor?
		if (!((i == 0) && noBoundaryCursors(item)) && !((i > 0) && noInnerCursors(item)))
			regs.append(cursorRegion);
		regs.append(itemRegion);
	}

	// Add trailing cursor region if not omitted
	if (!noBoundaryCursors(item))
	{
		QRect trailing;
		if (horizontal && forward_)
			trailing.setRect(last, elementBoundary.top(), midArea.right() + 1 - last, elementBoundary.height());
		else if (horizontal && !forward_)
			trailing.setRect(midArea.left(), elementBoundary.top(), last - midArea.left(), elementBoundary.height());
		else if (!horizontal && forward_)
			trailing.setRect(elementBoundary.left(), last,  elementBoundary.width(), midArea.bottom() + 1 - last);
		else trailing.setRect(elementBoundary.left(), midArea.top(),  elementBoundary.width(), last - midArea.top());

		adjustCursorRegionToAvoidZeroSize(trailing, horizontal, false, !extraCursors);

		regs.append(ItemRegion(trailing));
		// Note below, that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(item, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		lc->setOwnerElement(this);
		regs.last().setCursor(lc);
		lc->setIndex(itemList.size());

		auto vSize = horizontal ? QSize(2, elementBoundary.height()) : QSize(elementBoundary.width(), 2);
		lc->setVisualizationSize(vSize);
		auto vPos = regs.last().region().topLeft();
		if (itemList.isEmpty())
		{
			// Center the cursors. The final + 1 is just for rounding, it looks more centered with it.
			if (horizontal) vPos.rx() += (regs.last().region().width() - vSize.width()) / 2 + 1;
			else vPos.ry() += (regs.last().region().height() - vSize.height()) / 2 + 1;
		}
		lc->setVisualizationPosition(vPos);

		lc->setRegion(trailing);
		if (!extraCursors) lc->setIsAtBoundary(true);
		if (notLocationEquivalentCursors(item)) lc->setNotLocationEquivalent(true);
	}

	// Finally add the two extra cursors if requested
	if (extraCursors)
	{
		QRect extra;

		// Front
		if (horizontal && forward_)
			extra.setRect(0, 0, midArea.left(), item->heightInLocal());
		else if (horizontal && !forward_)
			extra.setRect(midArea.right() + 1, 0, item->widthInLocal() - midArea.right() - 1, item->heightInLocal());
		else if (!horizontal && forward_)
			extra.setRect(0, 0,  item->widthInLocal(), midArea.top());
		else
			extra.setRect(0, midArea.bottom() + 1, item->widthInLocal(), item->heightInLocal() - midArea.bottom() - 1);

		adjustCursorRegionToAvoidZeroSize(extra, horizontal, true, false);

		regs.append(ItemRegion(extra));
		// Note below, that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(item, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		lc->setOwnerElement(this);
		regs.last().setCursor(lc);
		lc->setIndex(-1);
		lc->setVisualizationPosition(regs.last().region().topLeft());
		lc->setVisualizationSize(horizontal ? QSize(2, item->heightInLocal()) : QSize(item->widthInLocal(), 2));
		lc->setRegion(extra);
		lc->setIsAtBoundary(true);
		if (notLocationEquivalentCursors(item)) lc->setNotLocationEquivalent(true);

		// Back
		if (horizontal && forward_)
			extra.setRect(midArea.right() + 1, 0, item->widthInLocal() - midArea.right() - 1, item->heightInLocal());
		else if (horizontal && !forward_)
			extra.setRect(0, 0, midArea.left(), item->heightInLocal());
		else if (!horizontal && forward_)
			extra.setRect(0, midArea.bottom() + 1, item->widthInLocal(), item->heightInLocal() - midArea.bottom() - 1);
		else
			extra.setRect(0, 0,  item->widthInLocal(), midArea.top());

		adjustCursorRegionToAvoidZeroSize(extra, horizontal, false, true);

		regs.append(ItemRegion(extra));
		// Note below, that a horizontal layout, means a vertical cursor
		lc = new LayoutCursor(item, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		lc->setOwnerElement(this);
		regs.last().setCursor(lc);
		lc->setIndex(itemList.size()+1);

		auto vSize = horizontal ? QSize(2, item->heightInLocal()) : QSize(item->widthInLocal(), 2);
		lc->setVisualizationSize(vSize);
		lc->setVisualizationPosition(regs.last().region().bottomRight() -QPoint(vSize.width(), vSize.height()));

		lc->setRegion(extra);
		lc->setIsAtBoundary(true);
		if (notLocationEquivalentCursors(item)) lc->setNotLocationEquivalent(true);
	}

	return regs;
}

inline void SequentialLayoutFormElement::adjustCursorRegionToAvoidZeroSize(QRect& region, bool horizontal, bool first,
		bool last)
{
	// Make sure there is at least some space for the cursor Region.
	if (horizontal && region.width() == 0)
	{
		if (forward_) region.adjust((first?0:-1), 0, (last?0:1), 0);
		else region.adjust((last?0:-1), 0, (first?0:1), 0);
	}
	if (!horizontal && region.height() == 0 )
	{
		if (forward_) region.adjust(0, (first?0:-1), 0, (last?0:1));
		else  region.adjust(0, (last?0:-1), 0, (first?0:1));
	}
}

bool SequentialLayoutFormElement::isEmpty(const Item* item) const
{
	for (auto i : dataForItem(item).items_)
		if (!i->isEmpty())
			return false;
	return true;
}

bool SequentialLayoutFormElement::elementOrChildHasFocus(Item* item) const
{
	if (LayoutFormElement::elementOrChildHasFocus(item))
		return true;
	for (auto i : dataForItem(item).items_)
		if (i->itemOrChildHasFocus())
			return true;
	return false;
}

int SequentialLayoutFormElement::focusedElementIndex(const Item* item) const
{
	auto& itemList = dataForItem(item).items_;

	for (int i = 0; i<itemList.size(); ++i)
		if (itemList[i]->itemOrChildHasFocus()) return i;

	return -1;
}

int SequentialLayoutFormElement::length(const Item* item) const
{
	return dataForItem(item).items_.size();
}

}
