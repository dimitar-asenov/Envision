/***********************************************************************************************************************
 * VBlockCFStyle.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VBlockCFStyle.h"

namespace ControlFlowVisualization {


void VBlockCFStyle::load(Visualization::StyleLoader& sl)
{
	ControlFlowItemStyle::load(sl);

	sl.load("statements", statements_);
}

}
