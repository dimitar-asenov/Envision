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
	spaceBetweenElements_(10)
{
}

PositionLayoutStyle* PositionLayoutStyle::getDefault()
{
	static PositionLayoutStyle s;
	return &s;
}

void PositionLayoutStyle::setSpaceBetweenElements(int space)
{
	if ( space >= 0 ) spaceBetweenElements_ = space;
}

void PositionLayoutStyle::load()
{
	LayoutStyle::load();
	Styles::load("spaceBetweenElements", spaceBetweenElements_);
}

}
