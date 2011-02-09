/***********************************************************************************************************************
 * VLibraryStyle.cpp
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VLibraryStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VLibraryStyle* VLibraryStyle::getDefault()
{
	static VLibraryStyle defaultStyle;
	return &defaultStyle;
}

void VLibraryStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("border", border_);
	Visualization::Styles::load("header", header_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("content", content_);
}

}
