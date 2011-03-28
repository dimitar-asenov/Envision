/***********************************************************************************************************************
 * VLibraryStyle.cpp
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/VLibraryStyle.h"

namespace OOVisualization {

void VLibraryStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("header", header_);
	sl.load("name", name_);
	sl.load("icon", icon_);
	sl.load("content", content_);
}

}
