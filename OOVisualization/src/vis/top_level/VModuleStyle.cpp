/***********************************************************************************************************************
 * VModuleStyle.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VModuleStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VModuleStyle::VModuleStyle()
{
}

VModuleStyle* VModuleStyle::getDefault()
{
	static VModuleStyle defaultStyle;
	return &defaultStyle;
}

void VModuleStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("borderStyle", borderStyle_);
	Visualization::Styles::load("headerStyle", headerStyle_);
	Visualization::Styles::load("contentStyle", contentStyle_);
}

}
