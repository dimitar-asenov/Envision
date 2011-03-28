/***********************************************************************************************************************
 * VStringLiteralStyle.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VStringLiteralStyle.h"

namespace OOVisualization {

void VStringLiteralStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("string", string_);
	sl.load("preSymbol", preSymbol_);
	sl.load("postSymbol", postSymbol_);
}

}
