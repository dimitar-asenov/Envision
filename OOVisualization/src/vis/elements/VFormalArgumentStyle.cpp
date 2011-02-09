/***********************************************************************************************************************
 * VFormalArgumentStyle.cpp
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/elements/VFormalArgumentStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VFormalArgumentStyle* VFormalArgumentStyle::getDefault()
{
	static VFormalArgumentStyle defaultStyle;
	return &defaultStyle;
}

void VFormalArgumentStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("container", container_);
	Visualization::Styles::load("name", name_);
}

}
