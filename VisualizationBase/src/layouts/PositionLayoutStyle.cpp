/***********************************************************************************************************************
 * PositionLayoutStyle.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PositionLayoutStyle.h"

namespace Visualization {

PositionLayoutStyle::PositionLayoutStyle() :
	spaceBetweenElements_(10), gridSize_(20)
{
}

void PositionLayoutStyle::load(StyleLoader& sl)
{
	LayoutStyle::load(sl);
	sl.load("leftInnerMargin", leftInnerMargin_);
	sl.load("rightInnerMargin", rightInnerMargin_);
	sl.load("topInnerMargin", topInnerMargin_);
	sl.load("bottomInnerMargin", bottomInnerMargin_);
	sl.load("spaceBetweenElements", spaceBetweenElements_);
	sl.load("gridSize", gridSize_);
}

}
