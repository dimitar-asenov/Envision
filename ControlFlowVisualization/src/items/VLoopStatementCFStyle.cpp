/***********************************************************************************************************************
 * VLoopStatementCFStyle.cpp
 *
 *  Created on: Feb 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VLoopStatementCFStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace ControlFlowVisualization {

VLoopStatementCFStyle* VLoopStatementCFStyle::getDefault()
{
	static VLoopStatementCFStyle defaultStyle;
	return &defaultStyle;
}

void VLoopStatementCFStyle::load()
{
	ControlFlowItemStyle::load();

	Visualization::Styles::load("initStep", initStep_);
	Visualization::Styles::load("condition", condition_);
	Visualization::Styles::load("updateStep", updateStep_);
	Visualization::Styles::load("body", body_);
}

}
