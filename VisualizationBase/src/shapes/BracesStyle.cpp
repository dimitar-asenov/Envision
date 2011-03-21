/***********************************************************************************************************************
 * BracesStyle.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/BracesStyle.h"
#include "Styles.h"

namespace Visualization {

BracesStyle* BracesStyle::getDefault()
{
	static BracesStyle defaultStyle;
	return &defaultStyle;
}

void BracesStyle::load()
{
	ShapeStyle::load();
	Styles::load("leftBracePen", leftBracePen_);
	Styles::load("leftBraceFont", leftBraceFont_);
	Styles::load("leftBrace", leftBrace_);
	Styles::load("rightBracePen", rightBracePen_);
	Styles::load("rightBraceFont", rightBraceFont_);
	Styles::load("rightBrace", rightBrace_);
}

}
