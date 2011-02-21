/***********************************************************************************************************************
 * VMethodCallStatementStyle.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VMethodCallStatementStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VMethodCallStatementStyle* VMethodCallStatementStyle::getDefault()
{
	static VMethodCallStatementStyle defaultStyle;
	return &defaultStyle;
}

void VMethodCallStatementStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("separator", separator_);
	Visualization::Styles::load("arguments", arguments_);
}

}
