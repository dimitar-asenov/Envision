/***********************************************************************************************************************
 * VArrayTypeStyle.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "types/VArrayTypeStyle.h"

namespace OOVisualization {

void VArrayTypeStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("symbol", symbol_);
}

}
