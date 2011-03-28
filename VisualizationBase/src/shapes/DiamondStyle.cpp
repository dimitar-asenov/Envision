/***********************************************************************************************************************
 * DiamondStyle.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/DiamondStyle.h"

namespace Visualization {

void DiamondStyle::load(StyleLoader& sl)
{
	ShapeStyle::load(sl);
	sl.load("backgroundBrush", background_);
	sl.load("shadowBrush", shadow_);
	sl.load("shadowXOffset", xShadowOffset_);
	sl.load("shadowYOffset", yShadowOffset_);
}

}
