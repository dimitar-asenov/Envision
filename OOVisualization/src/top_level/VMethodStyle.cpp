/***********************************************************************************************************************
 * VMethodStyle.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/VMethodStyle.h"

namespace OOVisualization {

void VMethodStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("nameDefault", nameDefault_);
	sl.load("namePublic", namePublic_);
	sl.load("namePrivate", namePrivate_);
	sl.load("nameProtected", nameProtected_);
	sl.load("nameStaticDefault", nameStaticDefault_);
	sl.load("nameStaticPublic", nameStaticPublic_);
	sl.load("nameStaticPrivate", nameStaticPrivate_);
	sl.load("nameStaticProtected", nameStaticProtected_);
	sl.load("icon", icon_);
	sl.load("header", header_);
	sl.load("content", content_);
	sl.load("arguments", arguments_);
	sl.load("results", results_);
}

}
