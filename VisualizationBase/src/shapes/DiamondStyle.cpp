/***********************************************************************************************************************
 * DiamondStyle.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/DiamondStyle.h"

#include "Styles.h"

namespace Visualization {

DiamondStyle::DiamondStyle() :
		background_(QColor(0xAA, 0xFF, 0xCC, 0xFF)), shadow_(Qt::NoBrush), xShadowOffset_(3), yShadowOffset_(3)
{
}

DiamondStyle* DiamondStyle::getDefault()
{
	static DiamondStyle defaultStyle;
	return &defaultStyle;
}

void DiamondStyle::load()
{
	ShapeStyle::load();
	Styles::load("backgroundBrush", background_);
	Styles::load("shadowBrush", shadow_);
	Styles::load("shadowXOffset", xShadowOffset_);
	Styles::load("shadowYOffset", yShadowOffset_);
}

}
