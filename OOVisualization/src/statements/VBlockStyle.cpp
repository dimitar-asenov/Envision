/***********************************************************************************************************************
 * VBlockStyle.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VBlockStyle.h"

namespace OOVisualization {

void VBlockStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("items", items_);
}

}
