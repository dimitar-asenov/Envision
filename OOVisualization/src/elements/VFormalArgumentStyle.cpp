/***********************************************************************************************************************
 * VFormalArgumentStyle.cpp
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VFormalArgumentStyle.h"

namespace OOVisualization {

void VFormalArgumentStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("name", name_);
}

}
