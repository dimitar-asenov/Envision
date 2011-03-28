/***********************************************************************************************************************
 * VIfStatementCFStyle.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VIfStatementCFStyle.h"

namespace ControlFlowVisualization {

void VIfStatementCFStyle::load(Visualization::StyleLoader& sl)
{
	ControlFlowItemStyle::load(sl);

	sl.load("condition", condition_);
	sl.load("thenBranch", thenBranch_);
	sl.load("elseBranch", elseBranch_);
}

}
