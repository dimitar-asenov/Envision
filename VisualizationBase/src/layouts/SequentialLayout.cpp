/***********************************************************************************************************************
 * SequentialLayout.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/SequentialLayout.h"
#include "shapes/Shape.h"

namespace Visualization {

SequentialLayout::SequentialLayout(Item* parent, SequentialLayoutStyle* style_) :
	Layout(parent), style(style_)
{
}

int SequentialLayout::length() const
{
	return items.size();
}

void SequentialLayout::append(Item* item)
{
	item->setParentItem(this);
	items.append(item);
	setNeedsUpdate();
}

void SequentialLayout::prepend(Item* item)
{
	item->setParentItem(this);
	items.prepend(item);
	setNeedsUpdate();
}

void SequentialLayout::insert(Item* item, int position)
{
	item->setParentItem(this);
	items.insert(position, item);
	setNeedsUpdate();
}

void SequentialLayout::setStyle(SequentialLayoutStyle* style_)
{
	style = style_;
	setNeedsUpdate();
}

void SequentialLayout::updateState()
{
	int w = style->leftMargin();
	int h = style->topMargin();

	// Get the maximum width and height of any element.
	int maxChildWidth = 0;
	int maxChildHeight = 0;
	int sizeWidth = style->leftMargin() + style->rightMargin();
	int sizeHeight = style->topMargin() + style->bottomMargin();

	for (int i = 0; i != items.size(); ++i)
	{
		if ( maxChildWidth < items[i]->width() ) maxChildWidth = items[i]->width();
		if ( maxChildHeight < items[i]->height() ) maxChildHeight = items[i]->height();

		sizeWidth += items[i]->width() + (i > 0 ? style->spaceBetweenElements() : 0);
		sizeHeight += items[i]->height() + (i > 0 ? style->spaceBetweenElements() : 0);
	}

	// Determine what sort of sequence we're building
	bool horizontal = style->direction() == SequentialLayoutStyle::LeftToRight || style->direction() == SequentialLayoutStyle::RightToLeft;
	bool forward = style->direction() == SequentialLayoutStyle::LeftToRight || style->direction() == SequentialLayoutStyle::TopToBottom;


	// Set the size
	if (horizontal) sizeHeight = style->topMargin() + maxChildHeight + style->bottomMargin();
	else sizeWidth = style->leftMargin() + maxChildWidth + style->rightMargin();

	if (getShape()) getShape()->setInnerSize(sizeWidth, sizeHeight);
	else
	{
		size.setWidth(sizeWidth);
		size.setHeight(sizeHeight);
		bounding_rect = QRectF();
	}

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

	QPoint offset;
	if (getShape()) offset = getShape()->contentPosition();

	// Set the positions of all elements
	for (int i = begin; i != end; i += step)
	{
		if ( horizontal )
		{
			int y = h;
			if ( style->alignment() == SequentialLayoutStyle::BottomAlignment ) y += maxChildHeight - items[i]->height();
			if ( style->alignment() == SequentialLayoutStyle::CenterAlignment ) y += (maxChildHeight - items[i]->height()) / 2;

			if ( i != begin ) w += style->spaceBetweenElements();
			items[i]->setPos(w + offset.x(), y + offset.y());
			w += items[i]->width();
		}
		else
		{
			int x = w;
			if ( style->alignment() == SequentialLayoutStyle::RightAlignment ) x += maxChildWidth - items[i]->width();
			if ( style->alignment() == SequentialLayoutStyle::CenterAlignment ) x += (maxChildWidth - items[i]->width()) / 2;

			if ( i != begin ) h += style->spaceBetweenElements();
			items[i]->setPos(x + offset.x(), h + offset.y());
			h += items[i]->height();
		}
	}
}

}
