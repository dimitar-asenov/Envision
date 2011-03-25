/***********************************************************************************************************************
 * EmptyMethodVisStyle.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/EmptyMethodVisStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace CustomMethodCall {

EmptyMethodVisStyle* EmptyMethodVisStyle::getDefault()
{
	static EmptyMethodVisStyle defaultStyle;
	return &defaultStyle;
}

void EmptyMethodVisStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("separator", separator_);
}

}
