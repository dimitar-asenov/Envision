/***********************************************************************************************************************
 * VListCFStyle.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VListCFStyle.h"

namespace ControlFlowVisualization {

void VListCFStyle::load(Visualization::StyleLoader& sl)
{
	ControlFlowItemStyle::load(sl);
	sl.load("sequence", sequence_);
}

}
