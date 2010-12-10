/***********************************************************************************************************************
 * Item.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Item.h"

namespace Visualization {

Item::Item(Item* parent) :
	QGraphicsItem(parent), sizeLimitChanged_(false)
{
}

QRectF Item::boundingRect() const
{
	return bounding_rect;
}

int Item::width() const
{
	return size.width();
}

int Item::height() const
{
	return size.height();
}

const QSize& Item::getSizeLimit() const
{
	return sizeLimit;
}

void Item::setSizeLimit(int limitWidth, int limitHeight)
{
	sizeLimit.setWidth(limitWidth);
	sizeLimit.setHeight(limitHeight);
	sizeLimitChanged_ = true;
}

void Item::setSizeWidthLimit(int limitWidth)
{
	sizeLimit.setWidth(limitWidth);
	sizeLimitChanged_ = true;
}

void Item::setSizeHeightLimit(int limitHeight)
{
	sizeLimit.setHeight(limitHeight);
	sizeLimitChanged_ = true;
}

bool Item::needsUpdate()
{
	return true;
}

void Item::updateSubtreeState()
{
	if ( needsUpdate() || sizeLimitChanged_ )
	{
		determineChildren();
		updateChildren();
		updateState();
		sizeLimitChanged_ = false;
	}
}

void Item::updateChildren()
{
	for (QList<QGraphicsItem *>::iterator child = childItems().begin(); child != childItems().end(); ++child)
	{
		Item* item = static_cast<Item*> (*child);
		item->updateSubtreeState();
	}
}

}
