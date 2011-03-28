/***********************************************************************************************************************
 * LayoutStyle.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/LayoutStyle.h"

namespace Visualization {

LayoutStyle::LayoutStyle(int leftMargin, int rightMargin, int topMargin, int bottomMargin) :
	leftMargin_(leftMargin), rightMargin_(rightMargin), topMargin_(topMargin), bottomMargin_(bottomMargin)
{
}

void LayoutStyle::setMargins(int marginSize)
{
	if ( marginSize >= 0 )
	{
		leftMargin_ = marginSize;
		rightMargin_ = marginSize;
		topMargin_ = marginSize;
		bottomMargin_ = marginSize;
	}
}

void LayoutStyle::setMargins(int left, int right, int top, int bottom)
{
	if ( left >= 0 ) leftMargin_ = left;
	if ( right >= 0 ) rightMargin_ = right;
	if ( top >= 0 ) topMargin_ = top;
	if ( bottom >= 0 ) bottomMargin_ = bottom;
}

void LayoutStyle::load(StyleLoader& sl)
{
	ItemStyle::load(sl);
	sl.load("leftMargin", leftMargin_);
	sl.load("topMargin", topMargin_);
	sl.load("rightMargin", rightMargin_);
	sl.load("bottomMargin", bottomMargin_);
}

}
