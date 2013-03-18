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
int AnchorLayoutConstraint::execute(Orientation orientation)
{
	Q_ASSERT(orientation != Orientation::Auto);

	if (orientation == Orientation::Horizontal)
	{
		int edgePosition = fixedElement_->pos().x() + offset_ +
									fixedElement_->size().width() * relativeFixedEdgePosition_;
		int placeX = edgePosition - placeElement_->size().width() * relativePlaceEdgePosition_;
		placeElement_->setPos(QPoint(placeX, placeElement_->pos().y()));
		return placeX;
	}
	else // orientation == Orientation::Vertical
	{
		int edgePosition = fixedElement_->pos().y() + offset_ +
									fixedElement_->size().height() * relativeFixedEdgePosition_;
		int placeY = edgePosition - placeElement_->size().height() * relativePlaceEdgePosition_;
		placeElement_->setPos(QPoint(placeElement_->pos().x(), placeY));
		return placeY;
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
