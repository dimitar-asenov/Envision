/***********************************************************************************************************************
 * ShapeStyle.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/ShapeStyle.h"

namespace Visualization {

ShapeStyle::ShapeStyle() :
	outline_(Qt::SolidPattern, 1)
{
}

ShapeStyle::~ShapeStyle()
{
}

void ShapeStyle::setOutline(const QPen& outline)
{
	outline_ = outline;
}

void ShapeStyle::load(StyleLoader& sl)
{
	sl.load("outline", outline_);
}

}
