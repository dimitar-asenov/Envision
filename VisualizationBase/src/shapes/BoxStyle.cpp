/***********************************************************************************************************************
 * BoxStyle.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/BoxStyle.h"

namespace Visualization {

BoxStyle::BoxStyle() :
		background_(QColor(0xAA, 0xFF, 0xCC, 0xFF)), corner_(RightAngleCorner), cornerRadius_(5), shadow_(Qt::NoBrush), xShadowOffset_(3), yShadowOffset_(3)
{
}

BoxStyle* BoxStyle::getDefault()
{
	static BoxStyle defaultStyle;
	return &defaultStyle;
}

void BoxStyle::setBackground(const QBrush& brush)
{
	background_ = brush;
}

void BoxStyle::setCorner(BoxStyle::CornerType type, int radius)
{
	corner_ = type;
	cornerRadius_ = radius;
}

void BoxStyle::setShadow(const QBrush& brush, int xOffset, int yOffset)
{
	shadow_ = brush;
	xShadowOffset_ = xOffset;
	yShadowOffset_ = yOffset;
}

}
