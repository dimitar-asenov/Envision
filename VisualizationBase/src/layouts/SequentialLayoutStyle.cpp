/***********************************************************************************************************************
 * SequentialLayoutStyle.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/SequentialLayoutStyle.h"
#include "Styles.h"

namespace Visualization {

SequentialLayoutStyle::SequentialLayoutStyle() :
	direction_(LeftToRight), alignment_(CenterAlignment), spaceBetweenElements_(2)
{
}

SequentialLayoutStyle* SequentialLayoutStyle::getDefault()
{
	static SequentialLayoutStyle s;
	return &s;
}

void SequentialLayoutStyle::setDirection(Direction direction)
{
	direction_ = direction;
}

void SequentialLayoutStyle::setAlignment(Alignment alignment)
{
	alignment_ = alignment;
}

void SequentialLayoutStyle::setSpaceBetweenElements(int space)
{
	if ( space >= 0 ) spaceBetweenElements_ = space;
}

void SequentialLayoutStyle::load()
{
	LayoutStyle::load();
	Styles::load("direction", (int&) direction_);
	Styles::load("alignment", (int&) alignment_);
	Styles::load("spaceBetweenElements", spaceBetweenElements_);
}

}
