/***********************************************************************************************************************
 * Item.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Item.h"

namespace Visualization {

Item::Item(Item* parent) :
	QGraphicsItem(parent)
{

}

QRectF Item::boundingRect() const
{
	return bounding_rect;
}

QRectF Item::logicalRect() const
{
	return logical_rect;
}

const QSize& Item::getSizeLimit() const
{
	return sizeLimit;
}

void Item::setSizeLimit(int limitWidth, int limitHeight)
{
	sizeLimit.setWidth(limitWidth);
	sizeLimit.setHeight(limitHeight);
}

void Item::setSizeWidthLimit(int limitWidth)
{
	sizeLimit.setWidth(limitWidth);
}

void Item::setSizeHeightLimit(int limitHeight)
{
	sizeLimit.setHeight(limitHeight);
}

}
