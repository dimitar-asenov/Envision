/***********************************************************************************************************************
 * VFormalResultStyle.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VFormalResultStyle.h"

namespace OOVisualization {

void VFormalResultStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("name", name_);
}
}
