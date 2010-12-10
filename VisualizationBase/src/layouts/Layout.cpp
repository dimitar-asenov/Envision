/***********************************************************************************************************************
 * Layout.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/Layout.h"

namespace Visualization {

Layout::Layout(Item* parent) :
	Item(parent), needsUpdate_(true)
{
}

void Layout::setNeedsUpdate()
{
	needsUpdate_ = true;
}

bool Layout::needsUpdate()
{
	return needsUpdate_;
}

void Layout::determineChildren()
{
}

void Layout::updateSubtreeState()
{
	Item::updateSubtreeState();
	needsUpdate_ = false;
}

void Layout::paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{

}

}
