/***********************************************************************************************************************
 * OperatorStyle.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/OperatorStyle.h"

namespace OOVisualization {

void OperatorStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("preSymbol", preSymbol_);
	sl.load("inSymbol", inSymbol_);
	sl.load("postSymbol", postSymbol_);
}

void OperatorSequenceStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("operators", seq_);
}

}
