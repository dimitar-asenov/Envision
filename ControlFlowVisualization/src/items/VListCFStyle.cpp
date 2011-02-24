/***********************************************************************************************************************
 * VListCFStyle.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VListCFStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace ControlFlowVisualization {

VListCFStyle* VListCFStyle::getDefault()
{
	static VListCFStyle defaultStyle;
	return &defaultStyle;
}

void VListCFStyle::load()
{
	ControlFlowItemStyle::load();
	Visualization::Styles::load("sequence", sequence_);
}

}
