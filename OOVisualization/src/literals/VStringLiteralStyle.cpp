/***********************************************************************************************************************
 * VStringLiteralStyle.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VStringLiteralStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VStringLiteralStyle* VStringLiteralStyle::getDefault()
{
	static VStringLiteralStyle defaultStyle;
	return &defaultStyle;
}

void VStringLiteralStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("string", string_);
	Visualization::Styles::load("preSymbol", preSymbol_);
	Visualization::Styles::load("postSymbol", postSymbol_);
}

}
