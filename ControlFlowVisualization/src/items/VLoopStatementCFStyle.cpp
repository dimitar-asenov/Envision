/***********************************************************************************************************************
 * VLoopStatementCFStyle.cpp
 *
 *  Created on: Feb 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VLoopStatementCFStyle.h"

namespace ControlFlowVisualization {

void VLoopStatementCFStyle::load(Visualization::StyleLoader& sl)
{
	ControlFlowItemStyle::load(sl);

	sl.load("initStep", initStep_);
	sl.load("condition", condition_);
	sl.load("updateStep", updateStep_);
	sl.load("body", body_);
}

}
