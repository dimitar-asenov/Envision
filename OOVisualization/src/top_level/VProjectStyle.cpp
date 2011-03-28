/***********************************************************************************************************************
 * VProjectStyle.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/VProjectStyle.h"

namespace OOVisualization {

void VProjectStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("header", header_);
	sl.load("name", name_);
	sl.load("icon", icon_);
	sl.load("content", content_);
}

}
