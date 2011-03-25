/***********************************************************************************************************************
 * SumMethodVisStyle.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/SumMethodVisStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace CustomMethodCall {

SumMethodVisStyle* SumMethodVisStyle::getDefault()
{
	static SumMethodVisStyle defaultStyle;
	return &defaultStyle;
}

void SumMethodVisStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("arguments", arguments_);
}

}
