/***********************************************************************************************************************
 * VMethodCallStatementStyle.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VMethodCallStatementStyle.h"

namespace OOVisualization {

void VMethodCallStatementStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("name", name_);
	sl.load("separator", separator_);
	sl.load("arguments", arguments_);
}

}
