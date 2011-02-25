/***********************************************************************************************************************
 * ControlFlowItemStyle.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/ControlFlowItemStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace ControlFlowVisualization {

ControlFlowItemStyle* ControlFlowItemStyle::getDefault()
{
	static ControlFlowItemStyle defaultStyle;
	return &defaultStyle;
}

void ControlFlowItemStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("showAsControlFlow", showAsControlFlow_);
	Visualization::Styles::load("pinLength", pinLength_);
	Visualization::Styles::load("pin", pin_);
}

}
