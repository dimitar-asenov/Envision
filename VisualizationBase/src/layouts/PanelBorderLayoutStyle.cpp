/***********************************************************************************************************************
 * PanelBorderLayoutStyle.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelBorderLayoutStyle.h"

namespace Visualization {

PanelBorderLayoutStyle::PanelBorderLayoutStyle() :
	leftInnerMargin_(5), rightInnerMargin_(5), topInnerMargin_(5), bottomInnerMargin_(5)
{
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

void PanelBorderLayoutStyle::load(StyleLoader& sl)
{
	LayoutStyle::load(sl);
	sl.load("topPanelStyle", topStyle_);
	sl.load("leftPanelStyle", leftStyle_);
	sl.load("bottomPanelStyle", bottomStyle_);
	sl.load("rightPanelStyle", rightStyle_);

	sl.load("leftInnerMargin", leftInnerMargin_);
	sl.load("rightInnerMargin", rightInnerMargin_);
	sl.load("topInnerMargin", topInnerMargin_);
	sl.load("bottomInnerMargin", bottomInnerMargin_);

	sl.load("isLeftProtrusionFixed", leftProtrusionFixed_);
	if (leftProtrusionFixed_) sl.load("leftProtrusion", leftProtrusion_);

	sl.load("shapeOnlyOnContent", shapeOnlyOnContent_);
}

}
