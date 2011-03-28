/***********************************************************************************************************************
 * BoxStyle.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/BoxStyle.h"

namespace Visualization {

void BoxStyle::load(StyleLoader& sl)
{
	ShapeStyle::load(sl);
	sl.load("backgroundBrush", background_);
	sl.load("cornerType", (int&)corner_);
	sl.load("cornerRadius", cornerRadius_);
	sl.load("shadowBrush", shadow_);
	sl.load("shadowXOffset", xShadowOffset_);
	sl.load("shadowYOffset", yShadowOffset_);
}

}
