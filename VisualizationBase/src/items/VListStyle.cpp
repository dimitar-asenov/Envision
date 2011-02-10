/***********************************************************************************************************************
 * VListStyle.cpp
 *
 *  Created on: Jan 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VListStyle.h"
#include "Styles.h"

namespace Visualization {

VListStyle::VListStyle()
{
}

VListStyle* VListStyle::getDefault()
{
	static VListStyle defaultStyle;
	return &defaultStyle;
}

void VListStyle::load()
{
	ItemStyle::load();
	Styles::load("itemsStyle", itemsStyle_);
	Styles::load("drawShapeWhenEmpty", drawShapeWhenEmpty_);
}

}
