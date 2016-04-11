/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

#include "ItemRegion.h"

namespace Visualization {

ItemRegion::ItemRegion(const QRect& region)
	: region_{region}, item_{nullptr}, cursor_{nullptr}
{
}

ItemRegion::~ItemRegion()
{
	item_ = nullptr;
	cursor_ = nullptr;
}

qreal ItemRegion::distanceTo(const QPointF& point)
{
	// Translate the region so that its upper left corner is at 0, 0
	QPointF p = point - region_.topLeft();
	qreal width = region_.width();
	qreal height = region_.height();

	if (p.y() < 0)
	{
		// Above
		if (p.x() < 0) return std::sqrt(p.y()*p.y() + p.x()*p.x()); // To the left
		else if (p.x() > width) return std::sqrt(p.y()*p.y() + (p.x()-width)*(p.x()-width)); // To the right
		else return -p.y(); // Directly above
	}
	else if (p.y() > height)
	{
		// Below
		if (p.x() < 0) return std::sqrt((p.y()-height)*(p.y()-height) + p.x()*p.x()); // To the left
		else if (p.x() > width)
			return std::sqrt((p.y()-height)*(p.y()-height) + (p.x()-width)*(p.x()-width)); // To the right
		else return p.y()-height; // Directly below
	}
	else
	{
		// Within the same height
		if (p.x() < 0) return -p.x(); // To the left
		else if (p.x() > width) return p.x()-width; // To the right
		else return 0; // Inside
	}
}

ItemRegion::PositionConstraints ItemRegion::satisfiedPositionConstraints(const QPoint& point) const
{
	// Translate the region so that its upper left corner is at 0, 0
	QPointF p = point - region_.topLeft();

	PositionConstraints constraints = NoConstraints;

	if ( p.y() < region_.height()/2) constraints |= Below;
	if ( p.y() > region_.height()/2) constraints |= Above;

	if ( p.x() < region_.width()/2) constraints |= RightOf;
	if ( p.x() > region_.width()/2) constraints |= LeftOf;

	if ( p.y() >= 0 && p.y() < region_.height() &&  p.x() >= 0 && p.x() < region_.width())
		constraints |= Overlap;

	return constraints;
}

bool ItemRegion::satisfiesConstraint(PositionConstraint constraint, QRect rect) const
{
	switch (constraint)
	{
		case NoConstraints: return true;
		case Below: return region_.bottom() > rect.bottom()
					&& region_.left() <= rect.right() && region_.right() >= rect.left();
		case Above: return region_.top() < rect.top()
					&& region_.left() <= rect.right() && region_.right() >= rect.left();
		case LeftOf: return region_.left() < rect.left()
					&& region_.top() <= rect.bottom() && region_.bottom() >= rect.top();
		case RightOf: return region_.right() > rect.right()
					&& region_.top() <= rect.bottom() && region_.bottom() >= rect.top();
		case Overlap: return region_.left() <= rect.right() && region_.right() >= rect.left()
					&& region_.top() <= rect.bottom() && region_.bottom() >= rect.top();
		default: Q_ASSERT(false); return false; // Unknown position constraint
	}
}

}
