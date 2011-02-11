/***********************************************************************************************************************
 * VFormalResultStyle.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/elements/VFormalResultStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VFormalResultStyle* VFormalResultStyle::getDefault()
{
	static VFormalResultStyle defaultStyle;
	return &defaultStyle;
}

void VFormalResultStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("container", container_);
	Visualization::Styles::load("name", name_);
}
}
