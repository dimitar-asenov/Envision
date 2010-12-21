/***********************************************************************************************************************
 * Item.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Item.h"
#include "ItemStyle.h"
#include "shapes/Shape.h"
#include "shapes/ShapeStyle.h"
#include "VisualizationException.h"

namespace Visualization {

Item::Item(Item* parent, const ItemStyle* style) :
	QGraphicsItem(parent), style_(style), shape_(NULL), needsUpdate_(true)
{
	if (style_ && style_->hasShape()) shape_ = style_->createShape(this);
}

Item::~Item()
{
	if ( shape_ )
	{
		delete shape_;
		shape_ = NULL;
	}
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

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( shape_ ) shape_->paint(painter, option, widget);
	else
		throw VisualizationException("Calling Item::paint without specifying a shape.");
}

}
