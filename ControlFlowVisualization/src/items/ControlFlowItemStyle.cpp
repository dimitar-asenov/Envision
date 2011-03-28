/***********************************************************************************************************************
 * ControlFlowItemStyle.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/ControlFlowItemStyle.h"

namespace ControlFlowVisualization {

void ControlFlowItemStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);
	sl.load("pinLength", pinLength_);
	sl.load("pin", pin_);
}

}
