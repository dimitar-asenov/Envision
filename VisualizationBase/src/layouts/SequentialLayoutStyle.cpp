/***********************************************************************************************************************
 * SequentialLayoutStyle.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/SequentialLayoutStyle.h"

namespace Visualization {

SequentialLayoutStyle::SequentialLayoutStyle() :
	direction_(LeftToRight), alignment_(CenterAlignment), spaceBetweenElements_(2)
{
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

void SequentialLayoutStyle::load(StyleLoader& sl)
{
	LayoutStyle::load(sl);
	sl.load("direction", (int&) direction_);
	sl.load("alignment", (int&) alignment_);
	sl.load("spaceBetweenElements", spaceBetweenElements_);
}

}
