/***********************************************************************************************************************
 * PanelLayoutStyle.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "PanelLayoutStyle.h"

namespace Visualization {

PanelLayoutStyle::PanelLayoutStyle()
{
}

static PanelLayoutStyle* PanelLayoutStyle::getDefault()
{
	static PanelLayoutStyle defaultStyle;
	return &defaultStyle;
}

void PanelLayoutStyle::setDirection(PanelLayoutDirection direction)
{
	direction_ = direction;
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
