/***********************************************************************************************************************
 * Icon.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/Icon.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(Icon)

Icon::Icon(Item* parent, const IconStyle* style) :
	Item(parent, style)
{
}

void Icon::determineChildren()
{
}

int Icon::xOffset() const
{
	if (hasShape()) return getShape()->contentLeft();
	else return 0;
}

int Icon::yOffset() const
{
	if (hasShape()) return getShape()->contentTop();
	else return 0;
}

void Icon::setInnerSize(int width, int height)
{
	if ( hasShape() ) getShape()->setInnerSize(width, height);
	else setSize(width, height);
}

}
