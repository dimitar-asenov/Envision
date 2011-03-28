/***********************************************************************************************************************
 * VVariableAccessStyle.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VVariableAccessStyle.h"

namespace OOVisualization {

void VVariableAccessStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("name", name_);
	sl.load("separator", separator_);
}

}
