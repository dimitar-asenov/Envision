/***********************************************************************************************************************
 * VClassStyle.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/VClassStyle.h"

namespace OOVisualization {

void VClassStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("nameDefault", nameDefault_);
	sl.load("namePublic", namePublic_);
	sl.load("namePrivate", namePrivate_);
	sl.load("nameProtected", nameProtected_);
	sl.load("icon", icon_);
	sl.load("header", header_);
	sl.load("baseClasses", baseClasses_);
	sl.load("content", content_);
	sl.load("fieldContainer", fieldContainer_);
	sl.load("publicFieldArea", publicFieldArea_);
	sl.load("privateFieldArea", privateFieldArea_);
	sl.load("protectedFieldArea", protectedFieldArea_);
	sl.load("defaultFieldArea", defaultFieldArea_);
}

}
