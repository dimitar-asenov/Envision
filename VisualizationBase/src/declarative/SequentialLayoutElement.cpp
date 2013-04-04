/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "SequentialLayoutElement.h"

#include "ModelBase/src/nodes/List.h"
#include "../renderer/ModelRenderer.h"
#include "../items/Item.h"
#include "../items/ItemRegion.h"
#include "../cursor/LayoutCursor.h"

namespace Visualization {

SequentialLayoutElement::SequentialLayoutElement()
{}

SequentialLayoutElement::~SequentialLayoutElement()
{
	for (auto list : itemListMap_)
		for (auto item : *list)
			SAFE_DELETE_ITEM(item);
}

void SequentialLayoutElement::computeSize(Item* item, int availableWidth, int availableHeight)
{
	auto& itemList = listForItem(item);

	int maxChildWidth = 0;
	int maxChildHeight = 0;

	bool horizontal = orientation_ == Qt::Horizontal;

	if ((horizontal && availableHeight > 0) || (!horizontal && availableWidth > 0))
	{
		// This sequential layout's size depends on its parent size and more space is available.
		if (horizontal) maxChildHeight = availableHeight - topMargin() - bottomMargin();
		else maxChildWidth = availableWidth - leftMargin() - rightMargin();
	}

	// Get the maximum width and height of any element and use those to compute the total width and height.
	int sizeWidth = 0;
	int sizeHeight = 0;
	for (auto i : itemList)
	{
		if (maxChildWidth < i->width()) maxChildWidth = i->width();
		if (maxChildHeight < i->height()) maxChildHeight = i->height();

		sizeWidth += i->width();
		sizeHeight += i->height();
	}
	if (!itemList.isEmpty())
	{
		sizeWidth += (itemList.size() - 1) * spaceBetweenElements();
		sizeHeight += (itemList.size() - 1) * spaceBetweenElements();
	}

	// minWidth and minHeight always apply to the dimension opposite of the direction.
	// In case there are no items in the list the minimum in the direction of the list also applies.
	if (horizontal)
	{
		if (maxChildHeight < minHeight_) maxChildHeight = minHeight_;
		if (itemList.isEmpty()) maxChildWidth = minWidth_;
	}
	else
	{
		if (maxChildWidth < minWidth_) maxChildWidth = minWidth_;
		if (itemList.isEmpty()) maxChildHeight = minHeight_;
	}

	// Update the geometry of children whose size varies
	for (auto i : itemList)
		if (i->sizeDependsOnParent())
		{
			if (horizontal) i->changeGeometry(0, maxChildHeight);
			else i->changeGeometry(maxChildWidth, 0);
		}

	// Set the size
	if (horizontal) sizeHeight = topMargin() + maxChildHeight + bottomMargin();
	else sizeWidth = leftMargin() + maxChildWidth + rightMargin();

	setSize(item, QSize(sizeWidth, sizeHeight));

	// Get the iteration parameters
	int begin;
	int end;
	int step;
	if (forward_)
	{
		begin = 0;
		end = itemList.size();
		step = 1;
	}
	else
	{
		begin = itemList.size() - 1;
		end = -1;
		step = -1;
	}

	int w = leftMargin();
	int h = topMargin();

	// Set the positions of all elements
	for (int i = begin; i != end; i += step)
		if (horizontal)
		{
			int y = h;
			if (alignment_ == LayoutStyle::Alignment::Bottom)
				y += maxChildHeight - itemList[i]->height();
			if (alignment_ == LayoutStyle::Alignment::Center)
				y += (maxChildHeight - itemList[i]->height()) / 2;

			if ( i != begin ) w += spaceBetweenElements();
			itemList[i]->setPos(w, y);
			w += itemList[i]->width();
		}
		else
		{
			int x = w;
			if (alignment_ == LayoutStyle::Alignment::Right)
				x += maxChildWidth - itemList[i]->width();
			if (alignment_ == LayoutStyle::Alignment::Center)
				x += (maxChildWidth - itemList[i]->width()) / 2;

			if ( i != begin ) h += spaceBetweenElements();
			itemList[i]->setPos(x, h);
			h += itemList[i]->height();
		}

}

void SequentialLayoutElement::setItemPositions(Item* item, int parentX, int parentY)
{
	for (auto i : listForItem(item))
		i->setPos(parentX + x(item) + i->x() + leftMargin(), parentY + y(item) + i->y() + topMargin());
}

void SequentialLayoutElement::synchronizeWithItem(Item* item)
{
	auto& itemList = listForItem(item);
	if (listNodeGetter_)
	{
		Model::List* listNode = listNodeGetter_();
		synchronizeWithNodes(item, listNode->children());
	}
	else if (nodeListGetter_)
		synchronizeWithNodes(item, nodeListGetter_());
	else
	{
		Q_ASSERT(itemListGetter_);
		synchronizeWithItems(item, itemListGetter_());
	}
	for (auto i : itemList) i->setParentItem(item);
}

bool SequentialLayoutElement::sizeDependsOnParent(const Item* item) const
{
	for (auto i : listForItem(item))
		if (i->sizeDependsOnParent())
			return true;
	return false;
}

void SequentialLayoutElement::destroyChildItems(Item* item)
{
	LayoutElement::destroyChildItems(item);
	if (itemListMap_.contains(item))
	{
		for (auto i : *itemListMap_.value(item))
			SAFE_DELETE_ITEM(i);
		itemListMap_.remove(item);
	}
}

QList<Item*>& SequentialLayoutElement::listForItem(const Item* item) const
{
	if (!itemListMap_.contains(item))
		itemListMap_.insert(item, new QList<Item*>());
	return *itemListMap_.value(item);
}

int SequentialLayoutElement::spaceBetweenElements()
{
	if (spaceBetweenElementsGetter_)
		return spaceBetweenElementsGetter_();
	return defaultSpaceBetweenElements_;
}

void SequentialLayoutElement::synchronizeWithNodes(Item* item, const QList<Model::Node*>& nodes)
{
	auto& itemList = listForItem(item);

	// Inserts elements that are not yet visualized and adjusts the order to match that in 'nodes'.
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (i >= itemList.size()) itemList.append(item->renderer()->render(item, nodes[i]));	// This node is new
		else if (itemList[i]->node() == nodes[i])	continue;	// This node is already there
		else
		{
			// This node might appear somewhere ahead, we should look for it
			bool found = false;
			for (int k = i + 1; k<itemList.size(); ++k)
			{
				if (itemList[k]->node() == nodes[i])
				{
					// We found this node, swap the visualizations
					swap(item, i, k);
					found = true;
					break;
				}
			}

			// The node was not found, insert a visualization here
			if (!found) itemList.insert(i, item->renderer()->render(item, nodes[i]));
		}
	}

	// Remove excess items
	while (itemList.size() > nodes.size()) itemList.removeAt(itemList.size()-1);
}

void SequentialLayoutElement::synchronizeWithItems(Item* item, const QList<Item*>& items)
{
	auto& itemList = listForItem(item);

	// Inserts elements that are not yet visualized and adjusts the order to match that in 'items'.
	for (int i = 0; i < items.size(); ++i)
	{
		if (i >= itemList.size()) itemList.append(items[i]);	// This item is new
		else if (itemList[i] == items[i])	continue;	// This item is already there
		else
		{
			// This item might appear somewhere ahead, we should look for it
			bool found = false;
			for (int k = i + 1; k<itemList.size(); ++k)
			{
				if (itemList[k] == items[i])
				{
					// We found this node, swap the visualizations
					swap(item, i, k);
					found = true;
					break;
				}
			}

			// The item was not found, insert a visualization here
			if (!found) itemList.insert(i, items[i]);
		}
	}

	// Remove excess items
	while (itemList.size() > items.size()) itemList.removeAt(itemList.size()-1);
}

void SequentialLayoutElement::swap(Item* item, int i, int j)
{
	auto& itemList = listForItem(item);

	Item* t = itemList[i];
	itemList[i] = itemList[j];
	itemList[j] = t;
	item->setUpdateNeeded(Item::StandardUpdate);
}

QList<ItemRegion> SequentialLayoutElement::regions(Item* item, int parentX, int parentY)
{
	QList<ItemRegion> regs;
	auto& itemList = listForItem(item);

	// If this layout is not visible return no regions
	if (itemList.isEmpty() && !hasCursorWhenEmpty_)
		return regs;

	QRect wholeArea = QRect(QPoint(x(item) + parentX, y(item) + parentY), size(item));
	QRect elementsArea = QRect(QPoint(wholeArea.left() + leftMargin(), wholeArea.top() + topMargin()),
										QPoint(wholeArea.right() - rightMargin(), wholeArea.bottom() - bottomMargin()));

	// This is the rectangle half-way between the bounding box of the layout and elementsArea.
	QRect midArea = QRect(QPoint(wholeArea.left() + leftMargin()/2, wholeArea.top() + topMargin()/2),
										QPoint(wholeArea.right() - rightMargin()/2, wholeArea.bottom() - bottomMargin()/2));

	bool horizontal = (orientation_ == Qt::Horizontal);

	int offset = (spaceBetweenElements() > 0) ? spaceBetweenElements()/2 : 1;

	int last = forward_ ?
			( horizontal ? midArea.left() : midArea.top()) :
			( horizontal ? midArea.right() + offset : midArea.bottom() + offset) ;

	for(int i = 0; i<itemList.size(); ++i)
	{
		ItemRegion cursorRegion;
		ItemRegion itemRegion;
		if (horizontal && forward_)
		{
			cursorRegion.setRegion(QRect(last, elementsArea.top(), itemList[i]->x() - last, elementsArea.height()));
			itemRegion.setRegion(QRect(itemList[i]->x(), elementsArea.top(), itemList[i]->width(), elementsArea.height()));
			last = itemList[i]->xEnd() + offset;
		}
		else if (horizontal && !forward_)
		{
			cursorRegion.setRegion(QRect(itemList[i]->xEnd()+1, elementsArea.top(), last, elementsArea.height()));
			itemRegion.setRegion(QRect(itemList[i]->x(), elementsArea.top(), itemList[i]->width(), elementsArea.height()));
			last = itemList[i]->x() - offset;
		}
		else if (!horizontal && forward_)
		{
			cursorRegion.setRegion(QRect(elementsArea.left(), last,  elementsArea.width(), itemList[i]->y() - last));
			itemRegion.setRegion(QRect(elementsArea.left(), itemList[i]->y(), elementsArea.width(),
												itemList[i]->height()));
			last = itemList[i]->yEnd() + offset;
		}
		else
		{
			cursorRegion.setRegion(QRect(elementsArea.left(), itemList[i]->yEnd()+1, elementsArea.width(), last));
			itemRegion.setRegion(QRect(elementsArea.left(), itemList[i]->y(), elementsArea.width(),
												itemList[i]->height()));
			last = itemList[i]->y() - offset;
		}

		itemRegion.setItem(itemList[i]);
		adjustCursorRegionToAvoidZeroSize(cursorRegion.region(), horizontal, i==0, false);

		// Note below, that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(item, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		lc->setOwnerElement(this);
		cursorRegion.setCursor(lc);
		lc->setIndex(i);
		lc->setVisualizationPosition(cursorRegion.region().topLeft());
		lc->setVisualizationSize(horizontal ? QSize(2, height(item)) : QSize(width(item), 2));
		lc->setOwnerElement(this);
		if (i==0) lc->setIsAtBoundary(true);

		cursorRegion.cursor()->setRegion(cursorRegion.region());
		if (notLocationEquivalentCursors_) lc->setNotLocationEquivalent(true);

		// Skip cursor?
		if (!((i == 0) && noBoundaryCursors_) && !((i > 0) && noInnerCursors_))
			regs.append(cursorRegion);
		regs.append(itemRegion);
	}

	// Add trailing cursor region if not omitted
	if (!noBoundaryCursors_)
	{
		QRect trailing;
		if (horizontal && forward_)
			trailing.setRect(last, elementsArea.top(), midArea.right() + 1 - last, elementsArea.height());
		else if (horizontal && !forward_)
			trailing.setRect(midArea.left(), elementsArea.top(), last - midArea.left(), elementsArea.height());
		else if (!horizontal && forward_)
			trailing.setRect(elementsArea.left(), last,  elementsArea.width(), midArea.bottom() + 1 - last);
		else trailing.setRect(elementsArea.left(), midArea.top(),  elementsArea.width(), last - midArea.top());

		adjustCursorRegionToAvoidZeroSize(trailing, horizontal, false, true);

		regs.append(ItemRegion(trailing));
		// Note below, that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(item, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		lc->setOwnerElement(this);
		regs.last().setCursor(lc);
		lc->setIndex(itemList.size());
		lc->setVisualizationPosition(regs.last().region().topLeft());
		lc->setVisualizationSize(horizontal ? QSize(2, height(item)) : QSize(width(item), 2));
		lc->setRegion(trailing);
		lc->setIsAtBoundary(true);
		if (notLocationEquivalentCursors_) lc->setNotLocationEquivalent(true);
	}
	return regs;
}

inline void SequentialLayoutElement::adjustCursorRegionToAvoidZeroSize(QRect& region, bool horizontal, bool first,
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

bool SequentialLayoutElement::elementOrChildHasFocus(Item* item) const
{
	if (LayoutElement::elementOrChildHasFocus(item))
		return true;
	for (auto i : listForItem(item))
		if (i->itemOrChildHasFocus())
			return true;
	return false;
}

int SequentialLayoutElement::focusedElementIndex(Item* item) const
{
	auto& itemList = listForItem(item);

	for (int i = 0; i<itemList.size(); ++i)
		if (itemList[i]->itemOrChildHasFocus()) return i;

	return -1;
}

}
