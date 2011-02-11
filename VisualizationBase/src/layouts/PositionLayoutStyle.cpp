/***********************************************************************************************************************
 * PositionLayoutStyle.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PositionLayoutStyle.h"
#include "Styles.h"

namespace Visualization {

PositionLayoutStyle::PositionLayoutStyle() :
	spaceBetweenElements_(10), gridSize_(20)
{
}

PositionLayoutStyle* PositionLayoutStyle::getDefault()
{
	static PositionLayoutStyle s;
	return &s;
}

void PositionLayoutStyle::load()
{
	LayoutStyle::load();
	Styles::load("leftInnerMargin", leftInnerMargin_);
	Styles::load("rightInnerMargin", rightInnerMargin_);
	Styles::load("topInnerMargin", topInnerMargin_);
	Styles::load("bottomInnerMargin", bottomInnerMargin_);
	Styles::load("spaceBetweenElements", spaceBetweenElements_);
	Styles::load("gridSize", gridSize_);
}

}
