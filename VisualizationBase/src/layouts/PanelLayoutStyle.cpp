/***********************************************************************************************************************
 * PanelLayoutStyle.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelLayoutStyle.h"

namespace Visualization {

PanelLayoutStyle::PanelLayoutStyle() : orientation_(HorizontalOrientation), alignment_(CenterAlignment), spaceBetweenElements_(5),
		leftMargin_(2), rightMargin_(2), topMargin_(2), bottomMargin_(2)
{
}

PanelLayoutStyle* PanelLayoutStyle::getDefault()
{
	static PanelLayoutStyle defaultStyle;
	return &defaultStyle;
}

void PanelLayoutStyle::setOrientation(PanelLayoutOrientation orientation)
{
	orientation_ = orientation;
}

void PanelLayoutStyle::setAlignment(PanelLayoutAlignment alignment)
{
	alignment_ = alignment;
}

void PanelLayoutStyle::setSpaceBetweenElements(int space)
{
	if ( space >= 0 ) spaceBetweenElements_ = space;
}

void PanelLayoutStyle::setMargins(int marginSize)
{
	if ( marginSize >= 0 )
	{
		leftMargin_ = marginSize;
		rightMargin_ = marginSize;
		topMargin_ = marginSize;
		bottomMargin_ = marginSize;
	}
}

void PanelLayoutStyle::setMargins(int left, int right, int top, int bottom)
{
	if ( left >= 0 ) leftMargin_ = left;
	if ( right >= 0 ) rightMargin_ = right;
	if ( top >= 0 ) topMargin_ = top;
	if ( bottom >= 0 ) bottomMargin_ = bottom;
}

}
