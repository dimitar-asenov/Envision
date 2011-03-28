/***********************************************************************************************************************
 * VArrayInitializerStyle.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VArrayInitializerStyle.h"

namespace OOVisualization {

void VArrayInitializerStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("values", values_);
}

}
