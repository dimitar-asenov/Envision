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

void Layout::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (getShape()) Item::paint(painter, option, widget);
}

}
