/***********************************************************************************************************************
 * VClassStyle.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VClassStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VClassStyle::VClassStyle()
{
}

VClassStyle* VClassStyle::getDefault()
{
	static VClassStyle defaultStyle;
	return &defaultStyle;
}

void VClassStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("border", border_);
	Visualization::Styles::load("topContainer", topContainer_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("visibility", visibility_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("header", header_);
	Visualization::Styles::load("content", content_);
	Visualization::Styles::load("fieldContainer", fieldContainer_);
	Visualization::Styles::load("publicFieldArea", publicFieldArea_);
	Visualization::Styles::load("privateFieldArea", privateFieldArea_);
	Visualization::Styles::load("protectedFieldArea", protectedFieldArea_);
	Visualization::Styles::load("defaultFieldArea", defaultFieldArea_);
}

}
