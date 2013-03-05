/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "AnchorLayoutConstraint.h"
#include "../items/Item.h"
#include "Element.h"

namespace Visualization {

AnchorLayoutConstraint::AnchorLayoutConstraint(float relativePlaceEdgePosition, Element* placeElement, int offset,
		float relativeFixedEdgePosition, Element* fixedElement)
: relativePlaceEdgePosition_{relativePlaceEdgePosition}, placeElement_{placeElement}, offset_{offset},
  relativeFixedEdgePosition_{relativeFixedEdgePosition}, fixedElement_{fixedElement}
{}

AnchorLayoutConstraint::~AnchorLayoutConstraint()
{}

/**
 * Calculates the position in the orientation axis of the element to be placed, assuming it's size was already
 * calculated, and the position on the orientation axis of the fixed element is already fixed.
 */
bool AnchorLayoutConstraint::execute(Orientation orientation, bool stretchPlaceElement, Item* item)
{
	Q_ASSERT(orientation != Orientation::Auto);

	if (orientation == Orientation::Horizontal)
	{
		int edgePosition = fixedElement_->pos().x() + offset_ +
									fixedElement_->size().width() * relativeFixedEdgePosition_;
		if (stretchPlaceElement)
		{
			Q_ASSERT(item != nullptr);
			Q_ASSERT(placeElement_->sizeDependsOnParent(item));
			Q_ASSERT(fixedElement_->sizeDependsOnParent(item));
			int placeMaxSizeX = (edgePosition - placeElement_->pos().x()) * relativePlaceEdgePosition_;
			if (placeElement_->size().width() < placeMaxSizeX)
			{
				placeElement_->computeSize(item, placeMaxSizeX, placeElement_->size().height());
				return true;
			}
			else if (placeElement_->size().width() > placeMaxSizeX)
			{
				edgePosition += placeElement_->size().width() - placeMaxSizeX;
				int newFixedElementSizeX = (edgePosition - fixedElement_->pos().x()) * relativeFixedEdgePosition_;
				fixedElement_->computeSize(item, newFixedElementSizeX, fixedElement_->size().height());
				return true;
			}
			return false;
		}
		else
		{
			int placeX = edgePosition - placeElement_->size().width() * relativePlaceEdgePosition_;
			if (placeX == placeElement_->pos().x())
				return false;
			else
			{
				placeElement_->setPos(QPoint(placeX, placeElement_->pos().y()));
				return true;
			}
		}
	}
	else // orientation == Orientation::Vertical
	{
		int edgePosition = fixedElement_->pos().y() + offset_ +
									fixedElement_->size().height() * relativeFixedEdgePosition_;
		if (stretchPlaceElement)
		{
			Q_ASSERT(item != nullptr);
			Q_ASSERT(placeElement_->sizeDependsOnParent(item));
			Q_ASSERT(fixedElement_->sizeDependsOnParent(item));
			int placeMaxSizeY = (edgePosition - placeElement_->pos().y()) * relativePlaceEdgePosition_;
			if (placeElement_->size().height() < placeMaxSizeY)
			{
				placeElement_->computeSize(item, placeElement_->size().width(), placeMaxSizeY);
				return true;
			}
			else if (placeElement_->size().height() > placeMaxSizeY)
			{
				edgePosition += placeElement_->size().height() - placeMaxSizeY;
				int newFixedElementSizeY = (edgePosition - fixedElement_->pos().y()) * relativeFixedEdgePosition_;
				fixedElement_->computeSize(item, fixedElement_->size().width(), newFixedElementSizeY);
				return true;
			}
			return false;
		}
		else
		{
			int placeY = edgePosition - placeElement_->size().height() * relativePlaceEdgePosition_;
			if (placeY == placeElement_->pos().y())
				return false;
			else
			{
				placeElement_->setPos(QPoint(placeElement_->pos().x(), placeY));
				return true;
			}
		}
	}
}

bool AnchorLayoutConstraint::dependsOn(AnchorLayoutConstraint* other, QList<AnchorLayoutConstraint*>& allConstraints)
{
	QList<AnchorLayoutConstraint*> dependsOn = {this};

	while (!dependsOn.empty())
	{
		for (auto c : allConstraints)
			if (c->placeElement() == dependsOn.first()->fixedElement())
			{
				if (c == other) return true;
				dependsOn.append(c);
			}
		dependsOn.removeFirst();
	}

	return false;
}

} /* namespace Visualization */
