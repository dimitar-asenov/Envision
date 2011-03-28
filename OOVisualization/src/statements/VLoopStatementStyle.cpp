/***********************************************************************************************************************
 * VLoopStatementStyle.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VLoopStatementStyle.h"

namespace OOVisualization {

void VLoopStatementStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("header", header_);
	sl.load("icon", icon_);
	sl.load("initStep", initStep_);
	sl.load("condition", condition_);
	sl.load("updateStep", updateStep_);
	sl.load("body", body_);
}

}
