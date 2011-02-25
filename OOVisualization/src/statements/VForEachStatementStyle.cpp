/***********************************************************************************************************************
 * VForEachStatementStyle.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VForEachStatementStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VForEachStatementStyle* VForEachStatementStyle::getDefault()
{
	static VForEachStatementStyle defaultStyle;
	return &defaultStyle;
}

void VForEachStatementStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("header", header_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("varContainer", varContainer_);
	Visualization::Styles::load("varName", varName_);
	Visualization::Styles::load("collection", collection_);
	Visualization::Styles::load("body", body_);
}

}
