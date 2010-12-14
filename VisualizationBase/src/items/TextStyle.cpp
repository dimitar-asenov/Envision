/***********************************************************************************************************************
 * TextStyle.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/TextStyle.h"

namespace Visualization {

TextStyle::TextStyle()
{
	font_.setPixelSize(15);
}

TextStyle* TextStyle::getDefault()
{
	static TextStyle defaultStyle;
	return &defaultStyle;
}

}
