/***********************************************************************************************************************
 * VIfStatementStyle.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VIfStatementStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VIfStatementStyle* VIfStatementStyle::getDefault()
{
	static VIfStatementStyle defaultStyle;
	return &defaultStyle;
}

void VIfStatementStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("header", header_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("condition", condition_);
	Visualization::Styles::load("content", content_);
	Visualization::Styles::load("thenBranch", thenBranch_);
	Visualization::Styles::load("elseBranch", elseBranch_);
}

}
