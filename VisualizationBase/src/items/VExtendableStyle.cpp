/***********************************************************************************************************************
 * VExtendableStyle.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VExtendableStyle.h"
#include "Styles.h"

namespace Visualization {

VExtendableStyle::VExtendableStyle() : expanded_(true)
{
}

VExtendableStyle* VExtendableStyle::getDefault()
{
	static VExtendableStyle defaultStyle;
	return &defaultStyle;
}

void VExtendableStyle::load()
{
	ItemStyle::load();
	Styles::load("expanded", expanded_);
	Styles::load("borderStyle", borderStyle_);
	Styles::load("headerStyle", headerStyle_);
	Styles::load("smallHeaderStyle", smallHeaderStyle_);
	Styles::load("attributesStyle", attributesStyle_);
}

}
