/***********************************************************************************************************************
 * VIfStatementStyle.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VIfStatementStyle.h"

namespace OOVisualization {

void VIfStatementStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("header", header_);
	sl.load("icon", icon_);
	sl.load("condition", condition_);
	sl.load("content", content_);
	sl.load("thenBranch", thenBranch_);
	sl.load("elseBranch", elseBranch_);
}

}
