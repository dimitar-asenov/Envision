/***********************************************************************************************************************
 * VMethodCFStyle.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VMethodCFStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace ControlFlowVisualization {

VMethodCFStyle* VMethodCFStyle::getDefault()
{
	static VMethodCFStyle defaultStyle;
	return &defaultStyle;
}

void VMethodCFStyle::load()
{
	VMethodStyle::load();
	Visualization::Styles::load("showAsControlFlow", showAsControlFlow_);
}

}
