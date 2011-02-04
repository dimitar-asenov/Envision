/***********************************************************************************************************************
 * VMethodStyle.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VMethodStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VMethodStyle::VMethodStyle()
{
}

VMethodStyle* VMethodStyle::getDefault()
{
	static VMethodStyle defaultStyle;
	return &defaultStyle;
}

void VMethodStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("border", border_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("nameStatic", nameStatic_);
	Visualization::Styles::load("visibility", visibility_);
	Visualization::Styles::load("header", header_);
	Visualization::Styles::load("nameContainer", nameContainer_);
	Visualization::Styles::load("content", content_);
	Visualization::Styles::load("arguments", arguments_);
	Visualization::Styles::load("results", results_);
}

}
