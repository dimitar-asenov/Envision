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

void SequentialLayout::remove(int index, bool deleteItem_)
{
	if (deleteItem_) SAFE_DELETE_ITEM( items[index]);
	items.remove(index);
	setUpdateNeeded();
}

void SequentialLayout::removeAll(bool deleteItems)
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

}
