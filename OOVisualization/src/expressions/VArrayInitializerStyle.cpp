/***********************************************************************************************************************
 * VArrayInitializerStyle.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VArrayInitializerStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VArrayInitializerStyle* VArrayInitializerStyle::getDefault()
{
	static VArrayInitializerStyle defaultStyle;
	return &defaultStyle;
}

void VArrayInitializerStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("values", values_);
}

}
