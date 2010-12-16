/***********************************************************************************************************************
 * PanelBorderLayoutStyle.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelBorderLayoutStyle.h"

namespace Visualization {

PanelBorderLayoutStyle::PanelBorderLayoutStyle() :
		shapeBorder_(MiddleBorder), leftInnerMargin_(5), rightInnerMargin_(5), topInnerMargin_(5), bottomInnerMargin_(5)
{
}

PanelBorderLayoutStyle* PanelBorderLayoutStyle::getDefault()
{
	static PanelBorderLayoutStyle defaultStyle;
	return &defaultStyle;
}

void PanelBorderLayoutStyle::setInnerMargins(int marginSize)
{
	if ( marginSize >= 0 )
	{
		leftInnerMargin_ = marginSize;
		rightInnerMargin_ = marginSize;
		topInnerMargin_ = marginSize;
		bottomInnerMargin_ = marginSize;
	}
}

void PanelBorderLayoutStyle::setInnerMargins(int left, int right, int top, int bottom)
{
	if ( left >= 0 ) leftInnerMargin_ = left;
	if ( right >= 0 ) rightInnerMargin_ = right;
	if ( top >= 0 ) topInnerMargin_ = top;
	if ( bottom >= 0 ) bottomInnerMargin_ = bottom;
}

}
