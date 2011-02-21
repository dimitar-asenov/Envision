/***********************************************************************************************************************
 * VClassStyle.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/VClassStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VClassStyle::VClassStyle()
{
}

VClassStyle* VClassStyle::getDefault()
{
	static VClassStyle defaultStyle;
	return &defaultStyle;
}

void VClassStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("nameDefault", nameDefault_);
	Visualization::Styles::load("namePublic", namePublic_);
	Visualization::Styles::load("namePrivate", namePrivate_);
	Visualization::Styles::load("nameProtected", nameProtected_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("header", header_);
	Visualization::Styles::load("baseClasses", baseClasses_);
	Visualization::Styles::load("content", content_);
	Visualization::Styles::load("fieldContainer", fieldContainer_);
	Visualization::Styles::load("publicFieldArea", publicFieldArea_);
	Visualization::Styles::load("privateFieldArea", privateFieldArea_);
	Visualization::Styles::load("protectedFieldArea", protectedFieldArea_);
	Visualization::Styles::load("defaultFieldArea", defaultFieldArea_);
}

}
