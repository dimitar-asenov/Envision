/***********************************************************************************************************************
 * TextStyle.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/TextStyle.h"

#include "Styles.h"

namespace Visualization {

TextStyle::TextStyle()
{
	font_.setPixelSize(15);
	selectionFont_.setPixelSize(15);
}

TextStyle* TextStyle::getDefault()
{
	static TextStyle defaultStyle;
	return &defaultStyle;
}

void TextStyle::load()
{
	ItemStyle::load();
	Styles::load("pen", pen_);
	Styles::load("font", font_);
	Styles::load("selectionPen", selectionPen_);
	Styles::load("selectionFont", selectionFont_);
}

}
