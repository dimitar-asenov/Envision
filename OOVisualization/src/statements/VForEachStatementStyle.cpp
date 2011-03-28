/***********************************************************************************************************************
 * VForEachStatementStyle.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VForEachStatementStyle.h"

namespace OOVisualization {

void VForEachStatementStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("header", header_);
	sl.load("icon", icon_);
	sl.load("varContainer", varContainer_);
	sl.load("varName", varName_);
	sl.load("collection", collection_);
	sl.load("body", body_);
}

}
