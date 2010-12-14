/***********************************************************************************************************************
 * Layout.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/Layout.h"

namespace Visualization {

Layout::Layout(Item* parent) :
	Item(parent)
{
}

void Layout::determineChildren()
{
}

bool Layout::needsUpdate()
{
	QList<QGraphicsItem *> children = childItems();
	for (QList<QGraphicsItem *>::iterator child = children.begin(); child != children.end(); ++child)
	{
		Item* item = static_cast<Item*> (*child);
		if (item->needsUpdate()) return true;
	}

	return false;
}

void Layout::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (getShape()) Item::paint(painter, option, widget);
}

}
