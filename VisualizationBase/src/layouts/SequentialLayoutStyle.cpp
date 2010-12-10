/***********************************************************************************************************************
 * SequentialLayoutStyle.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/SequentialLayoutStyle.h"

namespace Visualization {

SequentialLayoutStyle::SequentialLayoutStyle() :
	direction_(LeftToRight), alignment_(CenterAlignment), spaceBetweenElements_(5), leftMargin_(0), rightMargin_(0), topMargin_(0), bottomMargin_(0)
{

}

SequentialLayoutStyle* SequentialLayoutStyle::getDefault()
{
	static SequentialLayoutStyle s;
	return &s;
}

SequentialLayoutStyle::SequntialLayoutDirection SequentialLayoutStyle::direction() const
{
	return direction_;
}

SequentialLayoutStyle::SequntialLayoutAlignment SequentialLayoutStyle::alignment() const
{
	return alignment_;
}

int SequentialLayoutStyle::spaceBetweenElements() const
{
	return spaceBetweenElements_;
}

int SequentialLayoutStyle::leftMargin() const
{
	return leftMargin_;
}

int SequentialLayoutStyle::rightMargin() const
{
	return rightMargin_;
}

int SequentialLayoutStyle::topMargin() const
{
	return topMargin_;
}

int SequentialLayoutStyle::bottomMargin() const
{
	return bottomMargin_;
}

void SequentialLayoutStyle::setDirection(SequntialLayoutDirection direction)
{
	direction_ = direction;
}

void SequentialLayoutStyle::setAlignment(SequntialLayoutAlignment alignment)
{
	alignment_ = alignment;
}

void SequentialLayoutStyle::setSpaceBetweenElements(int space)
{
	if ( space >= 0 ) spaceBetweenElements_ = space;
}

void SequentialLayoutStyle::setMargins(int marginSize)
{
	if ( marginSize >= 0 )
	{
		leftMargin_ = marginSize;
		rightMargin_ = marginSize;
		topMargin_ = marginSize;
		bottomMargin_ = marginSize;
	}
}

void SequentialLayoutStyle::setMargins(int left, int right, int top, int bottom)
{
	if ( left >= 0 ) leftMargin_ = left;
	if ( right >= 0 ) rightMargin_ = right;
	if ( top >= 0 ) topMargin_ = top;
	if ( bottom >= 0 ) bottomMargin_ = bottom;
}

}
