/***********************************************************************************************************************
 * VFieldStyle.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VFieldStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VFieldStyle::VFieldStyle()
{
}

VFieldStyle* VFieldStyle::getDefault()
{
	static VFieldStyle defaultStyle;
	return &defaultStyle;
}

void VFieldStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("nameDefault", nameDefault_);
	Visualization::Styles::load("namePublic", namePublic_);
	Visualization::Styles::load("namePrivate", namePrivate_);
	Visualization::Styles::load("nameProtected", nameProtected_);
	Visualization::Styles::load("nameStaticDefault", nameStaticDefault_);
	Visualization::Styles::load("nameStaticPublic", nameStaticPublic_);
	Visualization::Styles::load("nameStaticPrivate", nameStaticPrivate_);
	Visualization::Styles::load("nameStaticProtected", nameStaticProtected_);
}

}
