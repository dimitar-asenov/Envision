/***********************************************************************************************************************
 * LayoutStyle.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/LayoutStyle.h"
#include "Styles.h"

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

void LayoutStyle::load()
{
	ItemStyle::load();
	Styles::load("leftMargin", leftMargin_);
	Styles::load("topMargin", topMargin_);
	Styles::load("rightMargin", rightMargin_);
	Styles::load("bottomMargin", bottomMargin_);
	Styles::load("drawShapeWhenEmpty", drawShapeWhenEmpty_);
}

}
