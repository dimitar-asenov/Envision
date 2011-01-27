/***********************************************************************************************************************
 * SequentialLayout.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/SequentialLayout.h"
#include "shapes/Shape.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( SequentialLayout )

SequentialLayout::SequentialLayout(Item* parent, const SequentialLayoutStyle* style) :
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
	items.remove(index);
	setUpdateNeeded();
}

void SequentialLayout::removeAll(Item* item, bool deleteItem)
{
	if (deleteItem) SAFE_DELETE_ITEM(item);
	for (int i = items.size() - 1; i>=0; --i)
		if (items.at(i) == item) items.remove(i);
	setUpdateNeeded();
}

void SequentialLayout::clear(bool deleteItems)
{
	if (deleteItems) for (int i = 0; i<items.size(); ++i) SAFE_DELETE_ITEM(items[i]);
	items.clear();
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
		if ( items[i]->childHasFocus()) return i;

	return -1;
}

bool SequentialLayout::focusChild(FocusTarget location)
{
	if (items.isEmpty()) return false;

	bool horizontal = style()->direction() == SequentialLayoutStyle::LeftToRight || style()->direction() == SequentialLayoutStyle::RightToLeft;
	bool forward = style()->direction() == SequentialLayoutStyle::LeftToRight || style()->direction() == SequentialLayoutStyle::TopToBottom;
	int current = focusedElementIndex();
	int max = items.size() - 1;

	Item* toFocus = NULL;

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

}
