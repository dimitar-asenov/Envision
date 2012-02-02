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
#include "layouts/LayoutRegion.h"
#include "cursor/LayoutCursor.h"
#include "shapes/Shape.h"
#include "items/ItemWithNode.h"
#include "ModelRenderer.h"

#include "ModelBase/headers/nodes/Node.h"

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
	setUpdateNeeded();
}

void SequentialLayout::prepend(Item* item)
{
	item->setParentItem(this);
	items.prepend(item);
	setUpdateNeeded();
}

void SequentialLayout::insert(Item* item, int position)
{
	item->setParentItem(this);
	items.insert(position, item);
	setUpdateNeeded();
}

void SequentialLayout::swap(int i, int j)
{
	Item* t = items[i];
	items[i] = items[j];
	items[j] = t;
	setUpdateNeeded();
}

void SequentialLayout::remove(int index, bool deleteItem_)
{
	if (deleteItem_) SAFE_DELETE_ITEM( items[index]);
	else items[index]->setParentItem(nullptr);
	items.remove(index);
	setUpdateNeeded();
}

void SequentialLayout::removeAll(Item* item, bool deleteItem)
{
	for (int i = items.size() - 1; i>=0; --i)
		if (items.at(i) == item) items.remove(i);
	if (deleteItem) SAFE_DELETE_ITEM(item);
	else if (item) item->setParentItem(nullptr);
	setUpdateNeeded();
}

void SequentialLayout::clear(bool deleteItems)
{
	for (int i = 0; i<items.size(); ++i)
	{
		if (deleteItems) SAFE_DELETE_ITEM(items[i]);
		else if (items[i]) items[i]->setParentItem(nullptr);
	}
	items.clear();
	setUpdateNeeded();
}

void SequentialLayout::synchronizeWithNodes(const QList<Model::Node*>& nodes, ModelRenderer* renderer)
{
	// Inserts elements that are not yet visualized and adjusts the order to match that in 'nodes'.
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (i >= items.size() ) append( renderer->render(nullptr, nodes[i]));	// This node is new
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
			if (!found ) insert( renderer->render(nullptr, nodes[i]), i);
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
		item = renderer()->render(nullptr, node);
		insert(item, ((position > length()) ? length() : position) );
	}

}

bool SequentialLayout::isEmpty() const
{
	for(int i = 0; i<items.size(); ++i)
		if (!items[i]->isEmpty()) return false;

	return true;
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
	bool horizontal = style()->direction() == SequentialLayoutStyle::LeftToRight || style()->direction() == SequentialLayoutStyle::RightToLeft;
	bool forward = style()->direction() == SequentialLayoutStyle::LeftToRight || style()->direction() == SequentialLayoutStyle::TopToBottom;

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
			if ( style()->alignment() == SequentialLayoutStyle::BottomAlignment ) y += maxChildHeight - items[i]->height();
			if ( style()->alignment() == SequentialLayoutStyle::CenterAlignment ) y += (maxChildHeight - items[i]->height()) / 2;

			if ( i != begin ) w += style()->spaceBetweenElements();
			items[i]->setPos(w + xOffset(), y + yOffset());
			w += items[i]->width();
		}
		else
		{
			int x = w;
			if ( style()->alignment() == SequentialLayoutStyle::RightAlignment ) x += maxChildWidth - items[i]->width();
			if ( style()->alignment() == SequentialLayoutStyle::CenterAlignment ) x += (maxChildWidth - items[i]->width()) / 2;

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

bool SequentialLayout::focusChild(FocusTarget location)
{
	if (items.isEmpty()) return false;

	bool horizontal = style()->direction() == SequentialLayoutStyle::LeftToRight || style()->direction() == SequentialLayoutStyle::RightToLeft;
	bool forward = style()->direction() == SequentialLayoutStyle::LeftToRight || style()->direction() == SequentialLayoutStyle::TopToBottom;
	int current = focusedElementIndex();
	int max = items.size() - 1;

	Item* toFocus = nullptr;

	switch (location)
	{
		case FOCUS_DEFAULT:
			{
				if (forward) toFocus = items.first();
				else toFocus = items.last();
			}
			break;
		case FOCUS_TOPMOST:
			{
				if (forward) toFocus = items.first();
				else toFocus = items.last();
			}
			break;
		case FOCUS_BOTTOMMOST:
			{
				if ( forward == horizontal) toFocus = items.first();
				else toFocus = items.last();
			}
			break;
		case FOCUS_LEFTMOST:
			{
				if ( forward ) toFocus = items.first();
				else toFocus = items.last();
			}
			break;
		case FOCUS_RIGHTMOST:
			{
				if ( (forward || horizontal) && !(forward && horizontal) ) toFocus = items.first();
				else toFocus = items.last();
			}
			break;
		case FOCUS_UP:
			{
				if (current >= 0 && !horizontal)
				{
					if (forward && current > 0) toFocus = items[current-1];
					else if (!forward && current < max) toFocus = items[current+1];
				}
			}
			break;
		case FOCUS_DOWN:
			{
				if (current >= 0 && !horizontal)
				{
					if (forward && current < max) toFocus = items[current+1];
					else if (!forward && current > 0) toFocus = items[current-1];
				}
			}
			break;
		case FOCUS_LEFT:
			{
				if (current >= 0 && horizontal)
				{
					if (forward && current > 0) toFocus = items[current-1];
					else if (!forward && current < max) toFocus = items[current+1];
				}
			}
			break;
		case FOCUS_RIGHT:
			{
				if (current >= 0 && horizontal)
				{
					if (forward && current < max) toFocus = items[current+1];
					else if (!forward && current > 0) toFocus = items[current-1];
				}
			}
			break;
	}

	return Item::focusChild(toFocus);
}

QList<LayoutRegion> SequentialLayout::regions()
{
	bool horizontal = style()->direction() == SequentialLayoutStyle::LeftToRight
							|| style()->direction() == SequentialLayoutStyle::RightToLeft;
	bool forward = style()->direction() == SequentialLayoutStyle::LeftToRight
						|| style()->direction() == SequentialLayoutStyle::TopToBottom;

	QList<LayoutRegion> regs;
	int last = forward ? 0 : horizontal ? width() : height();
	for(int i = 0; i<items.size(); ++i)
	{
		LayoutRegion cursorRegion;
		LayoutRegion itemRegion;
		if (horizontal && forward)
		{
			cursorRegion.setRegion(QRect(last, 0, items[i]->x() - last, height()));
			itemRegion.setRegion(QRect(items[i]->x(), 0, items[i]->width(), height()));
			last = items[i]->xEnd();
		}
		else if (horizontal && !forward)
		{
			cursorRegion.setRegion(QRect(items[i]->xEnd(), 0, last, height()));
			itemRegion.setRegion(QRect(items[i]->x(), 0, items[i]->width(), height()));
			last = items[i]->x();
		}
		else if (!horizontal && forward)
		{
			cursorRegion.setRegion(QRect(0, last,  width(), items[i]->y() - last));
			itemRegion.setRegion(QRect(0, items[i]->y(), width(), items[i]->height()));
			last = items[i]->yEnd();
		}
		else
		{
			cursorRegion.setRegion(QRect(0, items[i]->yEnd(),  width(), last));
			itemRegion.setRegion(QRect(0, items[i]->y(), width(), items[i]->height()));
			last = items[i]->y();
		}

		itemRegion.setChild(items[i]);
		cursorRegion.setCursor(new LayoutCursor(this));
		cursorRegion.cursor()->setIndex(i);
		cursorRegion.cursor()->setVisualizationPosition(cursorRegion.region().topLeft());
		cursorRegion.cursor()->setVisualizationSize(horizontal ? QSize(2, height()) : QSize(width(), 2));

		regs.append(cursorRegion);
		regs.append(itemRegion);
	}

	// Add trailing cursor region
	QRect trailing;
	if (horizontal && forward) trailing.setRect(last, 0, width() - last, height());
	else if (horizontal && !forward) trailing.setRect(0, 0, last, height());
	else if (!horizontal && forward) trailing.setRect(0, last,  width(), height() - last);
	else trailing.setRect(0, 0,  width(), last);

	regs.append(LayoutRegion(trailing));
	regs.last().setCursor(new LayoutCursor(this));
	regs.last().cursor()->setIndex(items.size());
	regs.last().cursor()->setVisualizationPosition(regs.last().region().topLeft());
	regs.last().cursor()->setVisualizationSize(horizontal ? QSize(2, height()) : QSize(width(), 2));

	return regs;
}

}
