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

#include "AnchorLayoutElement.h"

namespace Visualization {

/*
 * AnchorLayoutElement
 */

AnchorLayoutElement::AnchorLayoutElement()
: elementList_{QList<Element*>()}, horizontalConstraints_{Orientation::Horizontal},
  verticalConstraints_{Orientation::Vertical}
{
	// TODO Auto-generated constructor stub

}

AnchorLayoutElement::~AnchorLayoutElement()
{
	// TODO Auto-generated destructor stub
}

AnchorLayoutElement* AnchorLayoutElement::put(PlaceEdge placeEdge, Element* placeElement, AtEdge atEdge,
		Element* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);
	Edge fixedEdge = static_cast<Edge>(atEdge);

	Orientation orientation = inferOrientation(edgeToBePlaced, fixedEdge);

	return put(orientation, relativePosition(edgeToBePlaced), placeElement, 0, relativePosition(fixedEdge),
					fixedElement);
}

AnchorLayoutElement* AnchorLayoutElement::put(PlaceEdge placeEdge, Element* placeElement, int offset, FromEdge fromEdge,
		Element* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);
	Edge fixedEdge = static_cast<Edge>(fromEdge);

	Orientation orientation = inferOrientation(edgeToBePlaced, fixedEdge);

	// compute correct offset
	if (fixedEdge == Edge::Left || fixedEdge == Edge::Top)
		offset = -offset;

	return put(orientation, relativePosition(edgeToBePlaced), placeElement, offset, relativePosition(fixedEdge),
					fixedElement);
}

AnchorLayoutElement* AnchorLayoutElement::put(PlaceEdge placeEdge, Element* placeElement, float relativeEdgePosition,
		Element* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);

	Orientation orientation = this->orientation(edgeToBePlaced);
	Q_ASSERT(orientation != Orientation::Auto);

	return put(orientation, relativePosition(edgeToBePlaced), placeElement, 0, relativeEdgePosition, fixedElement);
}

void AnchorLayoutElement::computeSize(Item* item, int /*availableWidth*/, int /*availableHeight*/)
{
	// TODO: what to do with the additional size?
	// compute size of each sub-element and set their position to (0, 0)
	for (int i=0; i<elementList_.length(); i++)
	{
		elementList_.at(i)->computeSize(item, 0, 0);
		elementList_.at(i)->setPos(QPoint(0, 0));
	}

	// place elements horizontally
	int minX = horizontalConstraints_.placeElements(item);

	// place elements vertically
	int minY = verticalConstraints_.placeElements(item);

	// adjust positions, such that the minimum on each axis is at left/right margin, and compute overall element width
	// and height
	int adjustmentX = minX * -1 + leftMargin();
	int adjustmentY = minY * -1 + topMargin();
	int maxX = 0;
	int maxY = 0;
	for (int i=0; i<elementList_.length(); i++)
	{
		Element* element = elementList_.at(i);
		element->setPos(QPoint(element->pos().x() + adjustmentX, element->pos().y() + adjustmentY));
		int rightEdge = element->pos().x() + element->size().width();
		int bottomEdge = element->pos().y() + element->size().height();
		if (rightEdge > maxX)
			maxX = rightEdge;
		if (bottomEdge > maxY)
			maxY = bottomEdge;
	}
	setSize(QSize(maxX + rightMargin(), maxY + bottomMargin()));
}

void AnchorLayoutElement::setItemPositions(Item* item, int parentX, int parentY)
{
	for(int i=0; i<elementList_.length(); i++)
		elementList_.at(i)->setItemPositions(item, parentX + pos().x(), parentY + pos().y());
}

void AnchorLayoutElement::synchronizeWithItem(Item* item)
{
	for(int i=0; i<elementList_.length(); i++)
		if (elementList_.at(i) != nullptr)
			elementList_.at(i)->synchronizeWithItem(item);
}

bool AnchorLayoutElement::sizeDependsOnParent(const Item* /*item*/) const
{
	// TODO: implement sizeDependsOnParent
	return false;
}

AnchorLayoutElement* AnchorLayoutElement::put(Orientation orientation, float relativePlaceEdgePosition,
		Element* placeElement, int offset, float relativeFixedEdgePosition, Element* fixedElement)
{
	Q_ASSERT(orientation != Orientation::Auto);

	if (!elementList_.contains(placeElement))
		elementList_.append(placeElement);
	if (!elementList_.contains(fixedElement))
		elementList_.append(fixedElement);

	if (orientation == Orientation::Horizontal)
		horizontalConstraints_.addConstraint(relativePlaceEdgePosition, placeElement, offset,
																relativeFixedEdgePosition, fixedElement);
	else // orientation == Orientation::Vertical
		verticalConstraints_.addConstraint(relativePlaceEdgePosition, placeElement, offset,
																relativeFixedEdgePosition, fixedElement);
	return this;
}

void AnchorLayoutElement::destroyChildItems(Item* item)
{
	for (int i = 0; i < elementList_.length(); ++i)
		elementList_.at(i)->destroyChildItems(item);
}

AnchorLayoutElement::Orientation AnchorLayoutElement::orientation(Edge edge)
{
	switch (edge) {
		case Edge::Left:
		case Edge::Right:
		case Edge::HCenter:
			return Orientation::Horizontal;
		case Edge::Top:
		case Edge::Bottom:
		case Edge::VCenter:
			return Orientation::Vertical;
		default:
			return Orientation::Auto;
	}
}

/**
 * Computes orientation of the two edges, fails if orientation cannot be inferred (both are Center) of if the edges
 * have conflicting orientations (e.g. Orientation::Left and Orientation::Top).
 *
 * @return Either Orientation::Horizontal or Orientation::Vertical
 */
AnchorLayoutElement::Orientation AnchorLayoutElement::inferOrientation(Edge firstEdge, Edge secondEdge)
{
	Orientation firstOrientation = orientation(firstEdge);
	Orientation secondOrientation = orientation(secondEdge);

	Q_ASSERT(firstOrientation != Orientation::Auto || secondOrientation != Orientation::Auto);

	if (firstOrientation != Orientation::Auto) {
		Q_ASSERT(firstOrientation == secondOrientation || secondOrientation == Orientation::Auto);
		return firstOrientation;
	}
	else // secondOrientation != Orientation::Auto && firstOrientation == Orientation::Auto
	{
		return secondOrientation;
	}
}

float AnchorLayoutElement::relativePosition(Edge edge)
{
	switch (edge) {
		case Edge::Left:
		case Edge::Top:
			return 0.0;
		case Edge::Right:
		case Edge::Bottom:
			return 1.0;
		default:
			return 0.5;
	}
}

/*
 * AnchorLayoutElement::AxisConstraints
 */

AnchorLayoutElement::AxisConstraints::AxisConstraints(Orientation orientation)
: orientation_{orientation}, constraints_{QList<Constraint*>()}
{}

AnchorLayoutElement::AxisConstraints::~AxisConstraints()
{}

void AnchorLayoutElement::AxisConstraints::addConstraint(float relativePlaceEdgePosition, Element* placeElement,
		int offset, float relativeFixedEdgePosition, Element* fixedElement)
{
	// check if element is not already assumed to be fixed/placed
	for (int i = 0; i < constraints_.length(); ++i)
	{
		Q_ASSERT(constraints_.at(i)->fixedElement() != placeElement);
		Q_ASSERT(constraints_.at(i)->placeElement() != placeElement);
	}

	constraints_.append(new Constraint(relativePlaceEdgePosition, placeElement, offset,
															relativeFixedEdgePosition, fixedElement));
}

int AnchorLayoutElement::AxisConstraints::placeElements(Item* /*item*/)
{
	// place elements on axis
	int minPos = 0;
	for (int i=0; i<constraints_.length(); i++)
	{
		int pos = constraints_.at(i)->execute(orientation_);
		if (pos < minPos)
			minPos = pos;
	}

	return minPos;
}

/*
 * AnchorLayoutElement::AxisConstraints::Constraint
 */

AnchorLayoutElement::AxisConstraints::Constraint::Constraint(float relativePlaceEdgePosition, Element* placeElement,
		int offset, float relativeFixedEdgePosition, Element* fixedElement)
: relativePlaceEdgePosition_{relativePlaceEdgePosition}, placeElement_{placeElement}, offset_{offset},
  relativeFixedEdgePosition_{relativeFixedEdgePosition}, fixedElement_{fixedElement}
{}

AnchorLayoutElement::AxisConstraints::Constraint::~Constraint()
{}

/**
 * Calculates the position in the orientation axis of the element to be placed, assuming it's size was already
 * calculated, and the position on the orientation axis of the fixed element is already fixed.
 */
int AnchorLayoutElement::AxisConstraints::Constraint::execute(Orientation orientation)
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

}
