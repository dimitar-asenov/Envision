/***********************************************************************************************************************
 * VListStyle.cpp
 *
 *  Created on: Jan 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VListStyle.h"

namespace Visualization {

VListStyle::VListStyle()
{
}

void VListStyle::load(StyleLoader& sl)
{
	ItemStyle::load(sl);
	sl.load("itemsStyle", itemsStyle_);
}

}
