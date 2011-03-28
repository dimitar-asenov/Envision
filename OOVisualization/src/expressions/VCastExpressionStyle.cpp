/***********************************************************************************************************************
 * VCastExpressionStyle.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VCastExpressionStyle.h"

namespace OOVisualization {

void VCastExpressionStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
}

}
