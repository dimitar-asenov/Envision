/***********************************************************************************************************************
 * VArrayTypeStyle.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "types/VArrayTypeStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VArrayTypeStyle* VArrayTypeStyle::getDefault()
{
	static VArrayTypeStyle defaultStyle;
	return &defaultStyle;
}

void VArrayTypeStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("symbol", symbol_);
}

}
