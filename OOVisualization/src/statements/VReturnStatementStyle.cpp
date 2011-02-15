/***********************************************************************************************************************
 * VReturnStatementStyle.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VReturnStatementStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VReturnStatementStyle* VReturnStatementStyle::getDefault()
{
	static VReturnStatementStyle defaultStyle;
	return &defaultStyle;
}

void VReturnStatementStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("container", container_);
	Visualization::Styles::load("symbol", symbol_);
	Visualization::Styles::load("values", values_);
}

}
