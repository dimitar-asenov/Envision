/***********************************************************************************************************************
 * VProjectStyle.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VProjectStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VProjectStyle::VProjectStyle()
{
}

VProjectStyle* VProjectStyle::getDefault()
{
	static VProjectStyle defaultStyle;
	return &defaultStyle;
}

void VProjectStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("borderStyle", borderStyle_);
	Visualization::Styles::load("headerStyle", headerStyle_);
	Visualization::Styles::load("contentStyle", contentStyle_);
}

}
