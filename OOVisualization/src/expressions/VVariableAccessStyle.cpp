/***********************************************************************************************************************
 * VVariableAccessStyle.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VVariableAccessStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VVariableAccessStyle* VVariableAccessStyle::getDefault()
{
	static VVariableAccessStyle defaultStyle;
	return &defaultStyle;
}

void VVariableAccessStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("separator", separator_);
}

}
