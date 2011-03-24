/***********************************************************************************************************************
 * FindMethodVisStyle.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/FindMethodVisStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace CustomMethodCall {

FindMethodVisStyle* FindMethodVisStyle::getDefault()
{
	static FindMethodVisStyle defaultStyle;
	return &defaultStyle;
}

void FindMethodVisStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("separator", separator_);
	Visualization::Styles::load("arguments", arguments_);
}

}
