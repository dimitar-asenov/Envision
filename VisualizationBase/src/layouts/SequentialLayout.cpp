/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * SequentialLayout.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/SequentialLayout.h"
#include "cursor/LayoutCursor.h"
#include "shapes/Shape.h"
#include "items/ItemWithNode.h"
#include "../renderer/ModelRenderer.h"

#include "ModelBase/src/nodes/Node.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( SequentialLayout, "layout" )

SequentialLayout::SequentialLayout(Item* parent, const StyleType* style) :
	Layout(parent, style)
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

void SequentialLayout::determineChildren()
{
	// All this is just needed in order to support changing the purpose of a child node.

	if (!scene() || needsUpdate() != FullUpdate) return Layout::determineChildren();

	QList<Model::Node*> nodes;
	for (auto i : items)
		if (i->node()) nodes.append(i->node());

	if (nodes.size() != items.size()) return Layout::determineChildren();

	clear(true);
	synchronizeWithNodes(nodes, scene()->renderer());
}

void SequentialLayout::updateGeometry(int, int)
{
	// Get the maximum width and height of any element.
	int maxChildWidth = 0;
	int maxChildHeight = 0;
	int sizeWidth = 0;
	int sizeHeight = 0;

	for (int i = 0; i != items.size(); ++i)
	{
		if ( maxChildWidth < items[i]->width() ) maxChildWidth = items[i]->width();
		if ( maxChildHeight < items[i]->height() ) maxChildHeight = items[i]->height();

		sizeWidth += items[i]->width() + (i > 0 ? style()->spaceBetweenElements() : 0);
		sizeHeight += items[i]->height() + (i > 0 ? style()->spaceBetweenElements() : 0);
	}

	// Determine what sort of sequence we're building
	bool horizontal = isHorizontal();
	bool forward = isForward();

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
			if ( style()->alignment() == SequentialLayoutStyle::BottomAlignment )
				y += maxChildHeight - items[i]->height();
			if ( style()->alignment() == SequentialLayoutStyle::CenterAlignment )
				y += (maxChildHeight - items[i]->height()) / 2;

			if ( i != begin ) w += style()->spaceBetweenElements();
			items[i]->setPos(w + xOffset(), y + yOffset());
			w += items[i]->width();
		}
		else
		{
			int x = w;
			if ( style()->alignment() == SequentialLayoutStyle::RightAlignment )
				x += maxChildWidth - items[i]->width();
			if ( style()->alignment() == SequentialLayoutStyle::CenterAlignment )
				x += (maxChildWidth - items[i]->width()) / 2;

			if ( i != begin ) h += style()->spaceBetweenElements();
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
	bool horizontal = isHorizontal();
	bool forward = isForward();

	QList<ItemRegion> regs;
	int last = forward ? 0 : horizontal ? width() : height();
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
		// Note below that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(this, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		cursorRegion.setCursor(lc);
		lc->setIndex(i);
		lc->setVisualizationPosition(cursorRegion.region().topLeft());
		lc->setVisualizationSize(horizontal ? QSize(2, height()) : QSize(width(), 2));
		if (i==0) lc->setIsAtBoundary(true);

		// Make sure there is at least some space for the cursor Region.
		if (horizontal && cursorRegion.region().width() == 0)
		{
			if (forward) cursorRegion.region().adjust((i>0?-1:0), 0, 1, 0);
			else cursorRegion.region().adjust(-1, 0, (i>0?1:0), 0);
		}
		if (!horizontal && cursorRegion.region().height() == 0 )
		{
			if (forward) cursorRegion.region().adjust(0, (i>0?-1:0), 0, 1);
			else  cursorRegion.region().adjust(0, -1, 0, (i>0?1:0));
		}

		cursorRegion.cursor()->setRegion(mapToScene(cursorRegion.region()).boundingRect().toRect());
		if (style()->notLocationEquivalentCursors()) lc->setNotLocationEquivalent(true);

		// Skip cursor?
		if (!((i == 0) && style()->noBoundaryCursors()) && !((i > 0) && style()->noInnerCursors()))
			regs.append(cursorRegion);
		regs.append(itemRegion);
	}

	// Add trailing cursor region if not omited


	if (!style()->noBoundaryCursors())
	{
		QRect trailing;
		if (horizontal && forward) trailing.setRect(last, 0, width() - last, height());
		else if (horizontal && !forward) trailing.setRect(0, 0, last, height());
		else if (!horizontal && forward) trailing.setRect(0, last,  width(), height() - last);
		else trailing.setRect(0, 0,  width(), last);

		// Make sure there is at least some space for the cursor Region.
		if (horizontal && trailing.width() == 0)
		{
			if (forward) trailing.adjust(-1, 0, 0, 0);
			else trailing.adjust(0, 0, 1, 0);
		}
		if (!horizontal && trailing.height() == 0 )
		{
			if (forward) trailing.adjust(0, -1, 0, 0);
			else  trailing.adjust(0, 0, 0, 1);
		}

		regs.append(ItemRegion(trailing));
		// Note below that a horizontal layout, means a vertical cursor
		auto lc = new LayoutCursor(this, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
		regs.last().setCursor(lc);
		lc->setIndex(items.size());
		lc->setVisualizationPosition(regs.last().region().topLeft());
		lc->setVisualizationSize(horizontal ? QSize(2, height()) : QSize(width(), 2));
		lc->setRegion(mapToScene(trailing).boundingRect().toRect());
		lc->setIsAtBoundary(true);
		if (style()->notLocationEquivalentCursors()) lc->setNotLocationEquivalent(true);
	}

	return regs;
}

}
