/***********************************************************************************************************************
 * VBlockStyle.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VBlockStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {


VBlockStyle* VBlockStyle::getDefault()
{
	static VBlockStyle defaultStyle;
	return &defaultStyle;
}

void VBlockStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("items", items_);
}

}
