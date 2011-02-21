/***********************************************************************************************************************
 * OperatorStyle.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/OperatorStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

OperatorStyle* OperatorStyle::getDefault()
{
	static OperatorStyle defaultStyle;
	return &defaultStyle;
}

void OperatorStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("preSymbol", preSymbol_);
	Visualization::Styles::load("inSymbol", inSymbol_);
	Visualization::Styles::load("postSymbol", postSymbol_);
}

OperatorSequenceStyle* OperatorSequenceStyle::getDefault()
{
	static OperatorSequenceStyle defaultStyle;
	return &defaultStyle;
}

void OperatorSequenceStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("operators", seq_);
}

}
