/***********************************************************************************************************************
 * VExtendableStyle.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VExtendableStyle.h"

namespace Visualization {

VExtendableStyle::VExtendableStyle() : expanded_(true)
{
}

void VExtendableStyle::load(StyleLoader& sl)
{
	ItemStyle::load(sl);
	sl.load("expanded", expanded_);
	sl.load("borderStyle", borderStyle_);
	sl.load("headerStyle", headerStyle_);
	sl.load("smallHeaderStyle", smallHeaderStyle_);
	sl.load("attributesStyle", attributesStyle_);
}

}
