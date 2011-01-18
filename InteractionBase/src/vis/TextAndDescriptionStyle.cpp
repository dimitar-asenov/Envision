/***********************************************************************************************************************
 * TextAndDescriptionStyle.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/TextAndDescriptionStyle.h"

#include "VisualizationBase/headers/Styles.h"

namespace Interaction {

TextAndDescriptionStyle* TextAndDescriptionStyle::getDefault()
{
	static TextAndDescriptionStyle defaultStyle;
	return &defaultStyle;
}

void TextAndDescriptionStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("text", text_);
	Visualization::Styles::load("description", description_);
}

}
