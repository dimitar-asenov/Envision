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

#include "AnchorLayoutAnchor.h"
#include "../items/Item.h"
#include "Element.h"

namespace Visualization {

AnchorLayoutAnchor::AnchorLayoutAnchor(float relativePlaceEdgePosition, Element* placeElement, int offset,
		float relativeFixedEdgePosition, Element* fixedElement)
: relativePlaceEdgePosition_{relativePlaceEdgePosition}, placeElement_{placeElement}, offset_{offset},
  relativeFixedEdgePosition_{relativeFixedEdgePosition}, fixedElement_{fixedElement}
{}

AnchorLayoutAnchor::~AnchorLayoutAnchor()
{}

/**
 * Calculates the position in the orientation axis of the element to be placed, assuming it's size was already
 * calculated, and the position on the orientation axis of the fixed element is already fixed.
 */
bool AnchorLayoutAnchor::execute(Orientation orientation, bool stretchPlaceElement, Item* item)
{
	Q_ASSERT(orientation != Orientation::Auto);

	int fixedElementPosition = 0;
	int fixedElementSize = 0;
	int placeElementPosition = 0;
	int placeElementSize = 0;

	if (orientation == Orientation::Horizontal)
	{
		fixedElementPosition = fixedElement_->pos().x();
		fixedElementSize = fixedElement_->size().width();
		placeElementPosition = placeElement_->pos().x();
		placeElementSize = placeElement_->size().width();
	}
	else // orientation == Orientation::Vertical
	{
		fixedElementPosition = fixedElement_->pos().y();
		fixedElementSize = fixedElement_->size().height();
		placeElementPosition = placeElement_->pos().y();
		placeElementSize = placeElement_->size().height();
	}

	int edgePosition = fixedElementPosition + offset_ + fixedElementSize * relativeFixedEdgePosition_;
	if (stretchPlaceElement)
	{
		Q_ASSERT(item != nullptr);
		Q_ASSERT(placeElement_->sizeDependsOnParent(item));
		Q_ASSERT(fixedElement_->sizeDependsOnParent(item));

		int placeElementMaxSize = (edgePosition - placeElementPosition * relativePlaceEdgePosition_);
		if (placeElementSize < placeElementMaxSize)
		{
			// stretch placeElement_
			if (orientation == Orientation::Horizontal)
				placeElement_->computeSize(item, placeElementMaxSize, placeElement_->size().height());
			else // orientation == Orientation::Vertical
				placeElement_->computeSize(item, placeElement_->size().width(), placeElementMaxSize);
			return true;
		}
		else if (placeElementSize > placeElementMaxSize)
		{
			// stretch fixedElement_
			edgePosition += placeElementSize - placeElementMaxSize;
			int newFixedElementSize = (edgePosition - fixedElementPosition) * relativeFixedEdgePosition_;
			if (orientation == Orientation::Horizontal)
				fixedElement_->computeSize(item, newFixedElementSize, fixedElement_->size().height());
			else // orientation == Orientation::Vertical
				fixedElement_->computeSize(item, fixedElement_->size().width(), newFixedElementSize);
			return true;
		}
		return false;
	}
	else
	{
		int newPosition = edgePosition - placeElementSize * relativePlaceEdgePosition_;
		if (newPosition == placeElementPosition)
			return false;
		else
		{
			if (orientation == Orientation::Horizontal)
				placeElement_->setPos(QPoint(newPosition, placeElement_->pos().y()));
			else // orientation == Orientation::Vertical
				placeElement_->setPos(QPoint(placeElement_->pos().x(), newPosition));
			return true;
		}
	}
}

bool AnchorLayoutAnchor::dependsOn(AnchorLayoutAnchor* other, QList<AnchorLayoutAnchor*>& allConstraints)
{
	QList<AnchorLayoutAnchor*> dependsOn = {this};

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
