/***********************************************************************************************************************
 * ClassIconStyle.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/ClassIconStyle.h"

#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

ClassIconStyle* ClassIconStyle::getDefault()
{
	static ClassIconStyle defaultStyle;
	return &defaultStyle;
}

void ClassIconStyle::load()
{
	IconStyle::load();
	Visualization::Styles::load("width", width_);
}

}
