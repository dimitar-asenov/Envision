/***********************************************************************************************************************
 * VNewExpressionStyle.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VNewExpressionStyle.h"

namespace OOVisualization {

void VNewExpressionStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("newSymbol", newSymbol_);
}

}
