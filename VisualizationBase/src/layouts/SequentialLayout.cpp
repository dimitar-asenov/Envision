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

#include "layouts/SequentialLayout.h"
#include "cursor/LayoutCursor.h"
#include "shapes/Shape.h"
#include "items/ItemWithNode.h"
#include "../renderer/ModelRenderer.h"

#include "ModelBase/src/nodes/Node.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( SequentialLayout, "layout" )

SequentialLayout::SequentialLayout(Item* parent, const StyleType* style) :
Super(parent, style), spaceBetweenElements_(-1)
{
}

SequentialLayout::~SequentialLayout()
{
	for (int i = 0; i<items.size(); i++) SAFE_DELETE_ITEM( items[i]);
}

int SequentialLayout::length() const
{
	return items.size();
}

void SequentialLayout::append(Item* item)
{
	item->setParentItem(this);
	items.append(item);
	setUpdateNeeded(StandardUpdate);
}

void SequentialLayout::prepend(Item* item)
{
	item->setParentItem(this);
	items.prepend(item);
	setUpdateNeeded(StandardUpdate);
}

void SequentialLayout::insert(Item* item, int position)
{
	item->setParentItem(this);
	items.insert(position, item);
	setUpdateNeeded(StandardUpdate);
}

void SequentialLayout::swap(int i, int j)
{
	Item* t = items[i];
	items[i] = items[j];
	items[j] = t;
	setUpdateNeeded(StandardUpdate);
}

void SequentialLayout::remove(int index, bool deleteItem_)
{
	if (deleteItem_) SAFE_DELETE_ITEM( items[index]);
	else items[index]->setParentItem(nullptr);
	items.remove(index);
	setUpdateNeeded(StandardUpdate);
}

void SequentialLayout::removeAll(Item* item, bool deleteItem)
{
	for (int i = items.size() - 1; i>=0; --i)
		if (items.at(i) == item) items.remove(i);
	if (deleteItem) SAFE_DELETE_ITEM(item);
	else if (item) item->setParentItem(nullptr);
	setUpdateNeeded(StandardUpdate);
}

void SequentialLayout::clear(bool deleteItems)
{
	for (int i = 0; i<items.size(); ++i)
	{
		if (deleteItems) SAFE_DELETE_ITEM(items[i]);
		else if (items[i]) items[i]->setParentItem(nullptr);
	}
	items.clear();
	setUpdateNeeded(StandardUpdate);
}

void SequentialLayout::synchronizeWithNodes(const QList<Model::Node*>& nodes, ModelRenderer* renderer)
{
	// Inserts elements that are not yet visualized and adjusts the order to match that in 'nodes'.
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (i >= items.size() ) append( renderer->render(this, nodes[i]));	// This node is new
		else if ( items[i]->node() == nodes[i] )	continue;	// This node is already there
		else
		{
			// This node might appear somewhere ahead, we should look for it
			bool found = false;
			for (int k = i + 1; k<items.size(); ++k)
			{
				if ( items[k]->node() == nodes[i] )
				{
					// We found this node, swap the visualizations
					swap(i, k);
					found = true;
					break;
				}
			}

			// The node was not found, insert a visualization here
			if (!found ) insert( renderer->render(this, nodes[i]), i);
		}
	}

	// Remove excess items
	while (items.size() > nodes.size()) remove(items.size()-1);
}

void SequentialLayout::synchronizeFirst(Item*& item, Model::Node* node)
{
	synchronizeMid(item, node, 0);
}

void SequentialLayout::synchronizeLast(Item*& item, Model::Node* node)
{
	synchronizeMid(item, node, length());
}

void SequentialLayout::synchronizeMid(Item*& item, Model::Node* node, int position)
{
	if (item && item->node() != node )
	{
		removeAll(item);
		item = nullptr;
	}

	if (!item && node)
	{
		item = renderer()->render(this, node);
		insert(item, ((position > length()) ? length() : position) );
	}

}

bool SequentialLayout::isEmpty() const
{
	for(int i = 0; i<items.size(); ++i)
		if (!items[i]->isEmpty()) return false;

	return true;
}

int SequentialLayout::spaceBetweenElements() const
{
	if (spaceBetweenElements_ >=0) return spaceBetweenElements_;
	else return style()->spaceBetweenElements();
}

void SequentialLayout::setSpaceBetweenElements(bool use, int space)
{
	spaceBetweenElements_ = use ? space : -1;
}

bool SequentialLayout::sizeDependsOnParent() const
{
	for (auto item : items) if (item->sizeDependsOnParent()) return true;
	return false;
}

void SequentialLayout::updateGeometry(int availableWidth, int availableHeight)
{
	int maxChildWidth = 0;
	int maxChildHeight = 0;

	// Determine what sort of sequence we're building
	bool horizontal = isHorizontal();
	bool forward = isForward();

	if ((horizontal && availableHeight > 0) || (!horizontal && availableWidth > 0))
	{
		// This sequential layout's size depends on its parent size and more space is available.
		QSize innerSize{availableWidth, availableHeight};
		if (hasShape())
		{
			innerSize = getShape()->innerSize(horizontal ? width() : availableWidth,
				horizontal ? availableHeight: height() );
		}

		if (horizontal) maxChildHeight = innerSize.height() - style()->topMargin() - style()->bottomMargin();
		else maxChildWidth = innerSize.width() - style()->leftMargin() - style()->rightMargin();
	}

	// Get the maximum width and height of any element and use those to compute the total width and height.
	int sizeWidth = 0;
	int sizeHeight = 0;
	for (int i = 0; i != items.size(); ++i)
	{
		if ( maxChildWidth < items[i]->width() ) maxChildWidth = items[i]->width();
		if ( maxChildHeight < items[i]->height() ) maxChildHeight = items[i]->height();

		sizeWidth += items[i]->width() + (i > 0 ? spaceBetweenElements() : 0);
		sizeHeight += items[i]->height() + (i > 0 ? spaceBetweenElements() : 0);
	}

	// minWidth and minHeight always apply to the dimension opposite of the direction.
	// In case there are no items in the list the minimum in the direction of the list also applies.
	if (horizontal)
	{
		if (maxChildHeight < style()->minHeight()) maxChildHeight = style()->minHeight();
		if (items.isEmpty() ) maxChildWidth = style()->minWidth();
	}
	else
	{
		if (maxChildWidth < style()->minWidth()) maxChildWidth = style()->minWidth();
		if (items.isEmpty()) maxChildHeight = style()->minHeight();
	}

	// Update the geometry of children whose size varies
	for (int i = 0; i != items.size(); ++i)
	{
		if (items[i]->sizeDependsOnParent())
		{
			if (horizontal) items[i]->changeGeometry(0, maxChildHeight);
			else items[i]->changeGeometry(maxChildWidth, 0);
		}
	}

	// Set the size
	if (horizontal) sizeHeight = style()->topMargin() + maxChildHeight + style()->bottomMargin();
	else sizeWidth = style()->leftMargin() + maxChildWidth + style()->rightMargin();

	setInnerSize(sizeWidth, sizeHeight);

	// Get the iteration parameters
	int begin;
	int end;
	int step;
	if ( forward )
	{
		begin = 0;
		end = items.size();
		step = 1;
	}
	else
	{
		begin = items.size() - 1;
		end = -1;
		step = -1;
	}

	int w = style()->leftMargin();
	int h = style()->topMargin();

	// Set the positions of all elements
	for (int i = begin; i != end; i += step)
	{
		if ( horizontal )
		{
			int y = h;
			if ( style()->alignment() == SequentialLayoutStyle::Alignment::Bottom )
				y += maxChildHeight - items[i]->height();
			if ( style()->alignment() == SequentialLayoutStyle::Alignment::Center )
				y += (maxChildHeight - items[i]->height()) / 2;

			if ( i != begin ) w += spaceBetweenElements();
			items[i]->setPos(w + xOffset(), y + yOffset());
			w += items[i]->width();
		}
		else
		{
			int x = w;
			if ( style()->alignment() == SequentialLayoutStyle::Alignment::Right )
				x += maxChildWidth - items[i]->width();
			if ( style()->alignment() == SequentialLayoutStyle::Alignment::Center )
				x += (maxChildWidth - items[i]->width()) / 2;

			if ( i != begin ) h += spaceBetweenElements();
			items[i]->setPos(x + xOffset(), h + yOffset());
			h += items[i]->height();
		}
	}
}

int SequentialLayout::focusedElementIndex() const
{
	for (int i = 0; i<items.size(); ++i)
		if ( items[i]->itemOrChildHasFocus()) return i;

	return -1;
}

QList<ItemRegion> SequentialLayout::regions()
{
	QList<ItemRegion> regs;

	// If this layout is not visible return no regions
	if (isEmpty() && !style()->hasCursorWhenEmpty() && (!hasShape() || !style()->drawShapeWhenEmpty()))
		return regs;

	bool extraCursors = hasShape() && style()->extraCursorsOutsideShape();
	QRect itemsArea;	// This is the bounding box of all items. If the layout has no shape, this would be the same as the
							// layout's bounding box. Otherwise this is the same as the inner part of the shape.
	if (extraCursors)
	{
		itemsArea = QRect(QPoint(getShape()->contentLeft(), getShape()->contentTop()),
				getShape()->innerSize(size().toSize()));
		itemsArea.adjust(style()->leftMargin(), style()->topMargin(), -style()->rightMargin(), -style()->bottomMargin());
	}
	else
		itemsArea = QRect( QPoint(0,0), size().toSize());

	bool horizontal = isHorizontal();
	bool forward = isForward();

	// This is the rectangle half-way between the bounding box of the layout and itemsArea.
	// When extraCursors is true, this indicates the boundary between the extra cursors and the inner cursors
	QRect midArea = QRect(QPoint(itemsArea.left()/2, itemsArea.top()/2),
			QPoint((itemsArea.right() + width()) / 2, (itemsArea.bottom() + height())/2));

	int last = forward ?
			( horizontal ? midArea.left() : midArea.top()) :
			( horizontal ? midArea.right() + 1 : midArea.bottom() + 1) ;

	for(int i = 0; i<items.size(); ++i)
	{
		ItemRegion cursorRegion;
		ItemRegion itemRegion;
		if (horizontal && forward)
		{
			cursorRegion.setRegion(QRect(last, 0, items[i]->x() - last, height()));
			itemRegion.setRegion(QRect(items[i]->x(), 0, items[i]->width(), height()));
			last = items[i]->xEnd() + 1;
		}
		else if (horizontal && !forward)
		{
			cursorRegion.setRegion(QRect(items[i]->xEnd()+1, 0, last, height()));
			itemRegion.setRegion(QRect(items[i]->x(), 0, items[i]->width(), height()));
			last = items[i]->x() - 1;
		}
		else if (!horizontal && forward)
		{
			cursorRegion.setRegion(QRect(0, last,  width(), items[i]->y() - last));
			itemRegion.setRegion(QRect(0, items[i]->y(), width(), items[i]->height()));
			last = items[i]->yEnd() + 1;
		}
		else
		{
			cursorRegion.setRegion(QRect(0, items[i]->yEnd()+1,  width(), last));
			itemRegion.setRegion(QRect(0, items[i]->y(), width(), items[i]->height()));
			last = items[i]->y() - 1;
		}

		itemRegion.setItem(items[i]);
		adjustCursorRegionToAvoidZeroSize(cursorRegion.region(), horizontal, forward, !extraCursors && i==0, false);

		// Note below, that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(this, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		cursorRegion.setCursor(lc);
		lc->setIndex(i);
		lc->setVisualizationPosition(cursorRegion.region().topLeft());
		lc->setVisualizationSize(horizontal ? QSize(2, height()) : QSize(width(), 2));
		if (i==0 && !extraCursors) lc->setIsAtBoundary(true);

		cursorRegion.cursor()->setRegion(cursorRegion.region());
		if (style()->notLocationEquivalentCursors()) lc->setNotLocationEquivalent(true);

		// Skip cursor?
		if (!((i == 0) && style()->noBoundaryCursorsInsideShape()) && !((i > 0) && style()->noInnerCursors()))
			regs.append(cursorRegion);
		regs.append(itemRegion);
	}

	// Add trailing cursor region if not omitted
	if (!style()->noBoundaryCursorsInsideShape())
	{
		QRect trailing;
		if (horizontal && forward) trailing.setRect(last, 0, midArea.right() + 1 - last, height());
		else if (horizontal && !forward) trailing.setRect(midArea.left(), 0, last - midArea.left(), height());
		else if (!horizontal && forward) trailing.setRect(0, last,  width(), midArea.bottom() + 1 - last);
		else trailing.setRect(0, midArea.top(),  width(), last - midArea.top());

		adjustCursorRegionToAvoidZeroSize(trailing, horizontal, forward, false, !extraCursors);

		regs.append(ItemRegion(trailing));
		// Note below, that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(this, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		regs.last().setCursor(lc);
		lc->setIndex(items.size());
		lc->setVisualizationPosition(regs.last().region().topLeft());
		lc->setVisualizationSize(horizontal ? QSize(2, height()) : QSize(width(), 2));
		lc->setRegion(trailing);
		if (!extraCursors) lc->setIsAtBoundary(true);
		if (style()->notLocationEquivalentCursors()) lc->setNotLocationEquivalent(true);
	}

	// Finally add the two extra cursors if requested
	if (extraCursors)
	{
		QRect extra;

		// Front
		if (horizontal && forward) extra.setRect(0, 0, midArea.left(), height());
		else if (horizontal && !forward) extra.setRect(midArea.right() + 1, 0, width() - midArea.right() - 1, height());
		else if (!horizontal && forward) extra.setRect(0, 0,  width(), midArea.top());
		else extra.setRect(0, midArea.bottom() + 1, width(), height() - midArea.bottom() - 1);

		adjustCursorRegionToAvoidZeroSize(extra, horizontal, forward, true, false);

		regs.append(ItemRegion(extra));
		// Note below, that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(this, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		regs.last().setCursor(lc);
		lc->setIndex(-1);
		lc->setVisualizationPosition(regs.last().region().topLeft());
		lc->setVisualizationSize(horizontal ? QSize(2, height()) : QSize(width(), 2));
		lc->setRegion(extra);
		lc->setIsAtBoundary(true);
		if (style()->notLocationEquivalentCursors()) lc->setNotLocationEquivalent(true);

		// Back
		if (horizontal && forward) extra.setRect(midArea.right() + 1, 0, width() - midArea.right() - 1, height());
		else if (horizontal && !forward) extra.setRect(0, 0, midArea.left(), height());
		else if (!horizontal && forward) extra.setRect(0, midArea.bottom() + 1, width(), height() - midArea.bottom() - 1);
		else extra.setRect(0, 0,  width(), midArea.top());

		adjustCursorRegionToAvoidZeroSize(extra, horizontal, forward, false, true);

		regs.append(ItemRegion(extra));
		// Note below, that a horizontal layout, means a vertical cursor
		lc = new LayoutCursor(this, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		regs.last().setCursor(lc);
		lc->setIndex(items.size()+1);
		lc->setVisualizationPosition(regs.last().region().topLeft());
		lc->setVisualizationSize(horizontal ? QSize(2, height()) : QSize(width(), 2));
		lc->setRegion(extra);
		lc->setIsAtBoundary(true);
		if (style()->notLocationEquivalentCursors()) lc->setNotLocationEquivalent(true);
	}

	return regs;
}

inline void SequentialLayout::adjustCursorRegionToAvoidZeroSize(QRect& region, bool horizontal, bool forward,
		bool first, bool last)
{
	// Make sure there is at least some space for the cursor Region.
	if (horizontal && region.width() == 0)
	{
		if (forward) region.adjust((first?0:-1), 0, (last?0:1), 0);
		else region.adjust((last?0:-1), 0, (first?0:1), 0);
	}
	if (!horizontal && region.height() == 0 )
	{
		if (forward) region.adjust(0, (first?0:-1), 0, (last?0:1));
		else  region.adjust(0, (last?0:-1), 0, (first?0:1));
	}
}

}
