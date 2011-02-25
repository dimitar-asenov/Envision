/***********************************************************************************************************************
 * VLoopStatementStyle.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VLoopStatementStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VLoopStatementStyle* VLoopStatementStyle::getDefault()
{
	static VLoopStatementStyle defaultStyle;
	return &defaultStyle;
}

void VLoopStatementStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("header", header_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("initStep", initStep_);
	Visualization::Styles::load("condition", condition_);
	Visualization::Styles::load("updateStep", updateStep_);
	Visualization::Styles::load("body", body_);
}

}
