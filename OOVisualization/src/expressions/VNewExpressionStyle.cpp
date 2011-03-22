/***********************************************************************************************************************
 * VNewExpressionStyle.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VNewExpressionStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VNewExpressionStyle* VNewExpressionStyle::getDefault()
{
	static VNewExpressionStyle defaultStyle;
	return &defaultStyle;
}

void VNewExpressionStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("newSymbol", newSymbol_);
}

}
