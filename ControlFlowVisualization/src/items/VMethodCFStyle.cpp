/***********************************************************************************************************************
 * VMethodCFStyle.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VMethodCFStyle.h"

namespace ControlFlowVisualization {

void VMethodCFStyle::load(Visualization::StyleLoader& sl)
{
	VMethodStyle::load(sl);
	sl.load("showAsControlFlow", showAsControlFlow_);
	sl.load("contentCF", contentCF_);
}

}
