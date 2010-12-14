/***********************************************************************************************************************
 * Item.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Item.h"
#include "shapes/Shape.h"
#include "shapes/ShapeStyle.h"
#include "VisualizationException.h"

namespace Visualization {

Item::Item(Item* parent, Shape *shape) :
	QGraphicsItem(parent), needsUpdate_(true), shape_(shape)
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

void Item::setSizeLimit(int limitWidth, int limitHeight)
{
	sizeLimit.setWidth(limitWidth);
	sizeLimit.setHeight(limitHeight);
	needsUpdate_ = true;
}

void Item::setSizeWidthLimit(int limitWidth)
{
	sizeLimit.setWidth(limitWidth);
	needsUpdate_ = true;
}

void Item::setSizeHeightLimit(int limitHeight)
{
	sizeLimit.setHeight(limitHeight);
	needsUpdate_ = true;
}

bool Item::needsUpdate()
{
	return needsUpdate_;
}

void Item::updateSubtreeState()
{
	if ( needsUpdate_ || needsUpdate())
	{
		determineChildren();
		updateChildren();
		updateState();
		needsUpdate_ = false;
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
	needsUpdate_ = true;
}

void Item::setShapeStyle(ShapeStyle* style)
{
	if (!shape_) throw VisualizationException("Trying to set the style of a shape for an item, but no shape was specified.");
	shape_->setStyle(style);
	needsUpdate_ = true;
}

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( shape_ ) shape_->paint(painter, option, widget);
	else
		throw VisualizationException("Calling Item::paint without specifying a shape.");
}

}
