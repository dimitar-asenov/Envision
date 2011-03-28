/***********************************************************************************************************************
 * VReturnStatementStyle.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VReturnStatementStyle.h"

namespace OOVisualization {

void VReturnStatementStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("symbol", symbol_);
	sl.load("values", values_);
}

}
