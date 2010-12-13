/***********************************************************************************************************************
 * Item.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Item.h"
#include "shapes/Shape.h"
#include "VisualizationException.h"

namespace Visualization {

Item::Item(Item* parent, Shape *shape) :
	QGraphicsItem(parent), sizeLimitChanged_(false), shape_(shape)
{
}

Item::~Item()
{
	if ( shape_ )
	{
		delete shape_;
		shape_ = NULL;
	}
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
	QList<QGraphicsItem *> children = childItems();
	for (QList<QGraphicsItem *>::iterator child = children.begin(); child != children.end(); ++child)
	{
		Item* item = static_cast<Item*> (*child);
		item->updateSubtreeState();
	}
}

void Item::setShape(Shape* shape)
{
	shape_ = shape;
}

Shape* Item::shape()
{
	return shape_;
}

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( shape_ ) shape_->paint(painter, option, widget);
	else
		throw VisualizationException("Calling Item::paint without specifying a shape.");
}

}
