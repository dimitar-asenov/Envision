/***********************************************************************************************************************
 * Box.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Box.h"

#include <QtGui/QPainter>

namespace Visualization {

Box::Box(Item* parent, int sub) :
	Item(parent), items(this)
{
	for (int i = 0; i < sub; ++i)
		items.append(new Box(this, sub - 1));
}

void Box::determineChildren()
{
}

void Box::updateState()
{
	size.setWidth(items.width());
	size.setHeight(items.height());

	bounding_rect.setRect(0,0, size.width(), size.height());
}

void Box::paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->drawRect(0, 0, size.width(), size.height());
}

}
