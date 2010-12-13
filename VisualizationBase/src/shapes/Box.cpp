/***********************************************************************************************************************
 * Box.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/Box.h"

#include <QtGui/QPainter>

namespace Visualization {

Box::Box(Item* parent) :
	Shape(parent)
{
}

void Box::update()
{
	setItemSize(width(), height());
	setItemBoundingRect(xOffset(), yOffset(), width(), height());
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->drawRect(xOffset(), yOffset(), width(), height());
}

}
