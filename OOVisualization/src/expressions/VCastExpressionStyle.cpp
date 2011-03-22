/***********************************************************************************************************************
 * VCastExpressionStyle.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VCastExpressionStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VCastExpressionStyle* VCastExpressionStyle::getDefault()
{
	static VCastExpressionStyle defaultStyle;
	return &defaultStyle;
}

void VCastExpressionStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
}

}
