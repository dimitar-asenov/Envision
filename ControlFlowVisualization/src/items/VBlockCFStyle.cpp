/***********************************************************************************************************************
 * VBlockCFStyle.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VBlockCFStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace ControlFlowVisualization {

VBlockCFStyle* VBlockCFStyle::getDefault()
{
	static VBlockCFStyle defaultStyle;
	return &defaultStyle;
}

void VBlockCFStyle::load()
{
	ControlFlowItemStyle::load();

	Visualization::Styles::load("statements", statements_);
}

}
