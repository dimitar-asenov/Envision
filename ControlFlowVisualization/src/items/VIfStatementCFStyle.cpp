/***********************************************************************************************************************
 * VIfStatementCFStyle.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VIfStatementCFStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace ControlFlowVisualization {

VIfStatementCFStyle* VIfStatementCFStyle::getDefault()
{
	static VIfStatementCFStyle defaultStyle;
	return &defaultStyle;
}

void VIfStatementCFStyle::load()
{
	ControlFlowItemStyle::load();

	Visualization::Styles::load("condition", condition_);
	Visualization::Styles::load("thenBranch", thenBranch_);
	Visualization::Styles::load("elseBranch", elseBranch_);
}

}
