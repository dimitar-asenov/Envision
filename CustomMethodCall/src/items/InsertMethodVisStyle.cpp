/***********************************************************************************************************************
 * InsertMethodVisStyle.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/InsertMethodVisStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace CustomMethodCall {

InsertMethodVisStyle* InsertMethodVisStyle::getDefault()
{
	static InsertMethodVisStyle defaultStyle;
	return &defaultStyle;
}

void InsertMethodVisStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("icon", icon_);
	Visualization::Styles::load("separator", separator_);
	Visualization::Styles::load("arguments", arguments_);
}

}
