/***********************************************************************************************************************
 * VModuleStyle.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/VModuleStyle.h"
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
	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("header", header_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("content", content_);
}

}
