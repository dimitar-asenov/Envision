/***********************************************************************************************************************
 * BracesStyle.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/BracesStyle.h"

namespace Visualization {

void BracesStyle::load(StyleLoader& sl)
{
	ShapeStyle::load(sl);
	sl.load("leftBracePen", leftBracePen_);
	sl.load("leftBraceFont", leftBraceFont_);
	sl.load("leftBrace", leftBrace_);
	sl.load("rightBracePen", rightBracePen_);
	sl.load("rightBraceFont", rightBraceFont_);
	sl.load("rightBrace", rightBrace_);
}

}
