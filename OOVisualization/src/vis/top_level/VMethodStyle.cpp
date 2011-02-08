/***********************************************************************************************************************
 * VMethodStyle.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VMethodStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VMethodStyle::VMethodStyle()
{
}

VMethodStyle* VMethodStyle::getDefault()
{
	static VMethodStyle defaultStyle;
	return &defaultStyle;
}

void VMethodStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("border", border_);
	Visualization::Styles::load("nameDefault", nameDefault_);
	Visualization::Styles::load("namePublic", namePublic_);
	Visualization::Styles::load("namePrivate", namePrivate_);
	Visualization::Styles::load("nameProtected", nameProtected_);
	Visualization::Styles::load("nameStaticDefault", nameStaticDefault_);
	Visualization::Styles::load("nameStaticPublic", nameStaticPublic_);
	Visualization::Styles::load("nameStaticPrivate", nameStaticPrivate_);
	Visualization::Styles::load("nameStaticProtected", nameStaticProtected_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("header", header_);
	Visualization::Styles::load("content", content_);
	Visualization::Styles::load("arguments", arguments_);
	Visualization::Styles::load("results", results_);
}

}
