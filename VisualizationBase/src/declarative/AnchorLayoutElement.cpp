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
: elementList_{QList<Element*>()}, horizontalConstraints_{QList<AnchorLayoutAnchor*>()},
  verticalConstraints_{QList<AnchorLayoutAnchor*>()}, horizontalHasCircularDependencies_{false},
  verticalHasCircularDependencies_{false}
{}

AnchorLayoutElement::~AnchorLayoutElement()
{
	// TODO: delete all contained elements
}

AnchorLayoutElement* AnchorLayoutElement::put(PlaceEdge placeEdge, Element* placeElement, AtEdge atEdge,
		Element* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);
	Edge fixedEdge = static_cast<Edge>(atEdge);

	AnchorLayoutAnchor::Orientation orientation = inferOrientation(edgeToBePlaced, fixedEdge);

	return put(orientation, relativePosition(edgeToBePlaced), placeElement, 0, relativePosition(fixedEdge),
					fixedElement);
}

AnchorLayoutElement* AnchorLayoutElement::put(PlaceEdge placeEdge, Element* placeElement, int offset, FromEdge fromEdge,
		Element* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);
	Edge fixedEdge = static_cast<Edge>(fromEdge);

	AnchorLayoutAnchor::Orientation orientation = inferOrientation(edgeToBePlaced, fixedEdge);

	// compute correct offset
	if (fixedEdge == Edge::Left || fixedEdge == Edge::Top) offset = -offset;

	return put(orientation, relativePosition(edgeToBePlaced), placeElement, offset, relativePosition(fixedEdge),
					fixedElement);
}

AnchorLayoutElement* AnchorLayoutElement::put(PlaceEdge placeEdge, Element* placeElement, float relativeEdgePosition,
		Element* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);

	AnchorLayoutAnchor::Orientation orientation = this->orientation(edgeToBePlaced);
	Q_ASSERT(orientation != AnchorLayoutAnchor::Orientation::Auto);

	return put(orientation, relativePosition(edgeToBePlaced), placeElement, 0, relativeEdgePosition, fixedElement);
}

void AnchorLayoutElement::computeSize(Item* item, int /*availableWidth*/, int /*availableHeight*/)
{
	// compute size of each sub-element and set their position to (0, 0)
	for (Element* element : elementList_)
	{
		element->computeSize(item, 0, 0);
		element->setPos(QPoint(0, 0));
	}

	// place elements horizontally
	int minX = placeElements(horizontalConstraints_, AnchorLayoutAnchor::Orientation::Horizontal, item);

	// place elements vertically
	int minY = placeElements(verticalConstraints_, AnchorLayoutAnchor::Orientation::Vertical, item);

	// adjust positions, such that the minimum on each axis is at left/right margin, and compute overall element width
	// and height
	int adjustmentX = minX * -1 + leftMargin();
	int adjustmentY = minY * -1 + topMargin();
	int maxX = 0;
	int maxY = 0;
	for (Element* element : elementList_)
	{
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
	for(Element* element : elementList_)
		element->setItemPositions(item, parentX + pos().x(), parentY + pos().y());
}

void AnchorLayoutElement::synchronizeWithItem(Item* item)
{
	for(Element* element : elementList_)
		if (element != nullptr) element->synchronizeWithItem(item);
}

bool AnchorLayoutElement::sizeDependsOnParent(const Item* /*item*/) const
{
	return false;
}

AnchorLayoutElement* AnchorLayoutElement::put(AnchorLayoutAnchor::Orientation orientation,
		float relativePlaceEdgePosition, Element* placeElement, int offset, float relativeFixedEdgePosition,
		Element* fixedElement)
{
	Q_ASSERT(orientation != AnchorLayoutAnchor::Orientation::Auto);

	if (!elementList_.contains(placeElement)) elementList_.append(placeElement);
	if (!elementList_.contains(fixedElement)) elementList_.append(fixedElement);

	if (orientation == AnchorLayoutAnchor::Orientation::Horizontal)
		addConstraint(horizontalConstraints_, orientation, relativePlaceEdgePosition, placeElement, offset,
																			relativeFixedEdgePosition, fixedElement);
	else // orientation == AnchorLayoutConstraint::Orientation::Vertical
		addConstraint(verticalConstraints_, orientation, relativePlaceEdgePosition, placeElement, offset,
																			relativeFixedEdgePosition, fixedElement);
	return this;
}

void AnchorLayoutElement::destroyChildItems(Item* item)
{
	for (Element* element : elementList_) element->destroyChildItems(item);
}

AnchorLayoutAnchor::Orientation AnchorLayoutElement::orientation(Edge edge)
{
	switch (edge) {
		case Edge::Left:
		case Edge::Right:
		case Edge::HCenter:
			return AnchorLayoutAnchor::Orientation::Horizontal;
		case Edge::Top:
		case Edge::Bottom:
		case Edge::VCenter:
			return AnchorLayoutAnchor::Orientation::Vertical;
		default:
			return AnchorLayoutAnchor::Orientation::Auto;
	}
}

/**
 * Computes orientation of the two edges, fails if orientation cannot be inferred (both are Center) of if the edges
 * have conflicting orientations (e.g. Orientation::Left and Orientation::Top).
 *
 * @return Either Orientation::Horizontal or Orientation::Vertical
 */
AnchorLayoutAnchor::Orientation AnchorLayoutElement::inferOrientation(Edge firstEdge, Edge secondEdge)
{
	AnchorLayoutAnchor::Orientation firstOrientation = orientation(firstEdge);
	AnchorLayoutAnchor::Orientation secondOrientation = orientation(secondEdge);

	Q_ASSERT(firstOrientation != AnchorLayoutAnchor::Orientation::Auto
			||secondOrientation != AnchorLayoutAnchor::Orientation::Auto);

	if (firstOrientation != AnchorLayoutAnchor::Orientation::Auto) {
		Q_ASSERT(firstOrientation == secondOrientation
				|| secondOrientation == AnchorLayoutAnchor::Orientation::Auto);
		return firstOrientation;
	}
	else  // secondOrientation != AnchorLayoutConstraint::Orientation::Auto
			// && firstOrientation == AnchorLayoutConstraint::Orientation::Auto
		return secondOrientation;
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

void AnchorLayoutElement::addConstraint(QList<AnchorLayoutAnchor*>& constraints,
		AnchorLayoutAnchor::Orientation orientation, float relativePlaceEdgePosition, Element* placeElement,
		int offset, float relativeFixedEdgePosition, Element* fixedElement)
{
	constraints.append(new AnchorLayoutAnchor(relativePlaceEdgePosition, placeElement, offset,
																	relativeFixedEdgePosition, fixedElement));
	sortConstraints(constraints, orientation);
}

int AnchorLayoutElement::placeElements(QList<AnchorLayoutAnchor*>& constraints,
		AnchorLayoutAnchor::Orientation orientation, Item* item)
{
	Q_ASSERT(orientation != AnchorLayoutAnchor::Orientation::Auto);
	bool axisHasCircularDependencies = (orientation == AnchorLayoutAnchor::Orientation::Horizontal
														&& horizontalHasCircularDependencies_)
												|| (orientation == AnchorLayoutAnchor::Orientation::Vertical
														&& verticalHasCircularDependencies_);

	int minPos = 0;
	QList<Element*> alreadyPlacedInThisRound;
	QList<float> placedAtEdgePosition;
	int maxIterations = 1000;
	bool somethingChanged = true;

	for (int i=0; i<1 || (axisHasCircularDependencies && (i<maxIterations) && somethingChanged); ++i)
	{
		somethingChanged = false;

		for (AnchorLayoutAnchor* c : constraints)
		{
			if (alreadyPlacedInThisRound.contains(c->placeElement()))
			{
				int index = alreadyPlacedInThisRound.indexOf(c->placeElement());
				float lastEdge = placedAtEdgePosition.at(index);
				if (c->relativePlaceEdgePosition() < lastEdge)
				{
					somethingChanged = c->execute(orientation) || somethingChanged;
					placedAtEdgePosition[index] = c->relativePlaceEdgePosition();
				}
				else
					somethingChanged = c->execute(orientation, true, item) || somethingChanged;
			}
			else
			{
				somethingChanged = c->execute(orientation) || somethingChanged;
				alreadyPlacedInThisRound.append(c->placeElement());
				placedAtEdgePosition.append(c->relativePlaceEdgePosition());
			}
			int pos = 0;
			if (orientation == AnchorLayoutAnchor::Orientation::Horizontal)
				pos = c->placeElement()->pos().x();
			else // orientation == AnchorLayoutConstraint::Orientation::Vertical
				pos = c->placeElement()->pos().y();
			if (pos < minPos) minPos = pos;
		}
	}

	return minPos;
}

void AnchorLayoutElement::sortConstraints(QList<AnchorLayoutAnchor*>& constraints,
		AnchorLayoutAnchor::Orientation orientation)
{
	// check if this orientation does not already have circular dependencies (if so, don't sort)
	Q_ASSERT(orientation != AnchorLayoutAnchor::Orientation::Auto);
	if (orientation == AnchorLayoutAnchor::Orientation::Horizontal && horizontalHasCircularDependencies_)
		return;
	else if (orientation == AnchorLayoutAnchor::Orientation::Vertical && verticalHasCircularDependencies_)
		return;

	QList<AnchorLayoutAnchor*> sortedConstraints;
	QList<Element*> elementQueue;

	// pre-sort, such that a dependencies among constraints with equal placeElements are satisfied
	for (auto c : constraints)
		if (!elementQueue.contains(c->placeElement()))
			elementQueue.append(c->placeElement());
	for (auto e : elementQueue)
	{
		QList<AnchorLayoutAnchor*> placeElementConstraints;
		for (auto c : constraints)
			if (c->placeElement() == e)
				placeElementConstraints.append(c);
		if (placeElementConstraints.length() > 1)
		qSort(placeElementConstraints.begin(), placeElementConstraints.end(),
				[](AnchorLayoutAnchor* c1, AnchorLayoutAnchor* c2)
				{return c1->relativePlaceEdgePosition() < c2->relativePlaceEdgePosition();});
		for (auto c : placeElementConstraints)
			sortedConstraints.append(c);
	}
	constraints = sortedConstraints;
	sortedConstraints.clear();
	elementQueue.clear();


	// find all constraints which have a fixed node that depends on nothing
	for (auto c1 : constraints)
	{
		bool dependsOnSomething = false;
		for (auto c2 : constraints)
			if (c1->dependsOn(c2, constraints))
			{
				dependsOnSomething = true;
				break;
			}
		if (!dependsOnSomething)
		{
			if (!elementQueue.contains(c1->fixedElement())) elementQueue.append(c1->fixedElement());
		}
	}

	// if elementQueue is empty, but constraints isn't, then there is a circular dependency
	if (elementQueue.empty() && !constraints.empty())
	{
		if (orientation == AnchorLayoutAnchor::Orientation::Horizontal)
			horizontalHasCircularDependencies_ = true;
		else // orientation == AnchorLayoutConstraint::Orientation::Vertical
			verticalHasCircularDependencies_ = true;
		return;
	}

	for (int elementIndex=0; elementIndex<elementQueue.length(); ++elementIndex)
	{
		for (auto c:constraints)
		{
			if (c->fixedElement() == elementQueue.at(elementIndex))
			{
				sortedConstraints.append(c);
				if (elementQueue.contains(c->placeElement()))
				{
					if (elementQueue.indexOf(c->placeElement()) <= elementIndex)
					{
						if (orientation == AnchorLayoutAnchor::Orientation::Horizontal)
							horizontalHasCircularDependencies_ = true;
						else // orientation == AnchorLayoutConstraint::Orientation::Vertical
							verticalHasCircularDependencies_ = true;
						return;
					}
				}
				else elementQueue.append(c->placeElement());
			}
		}
	}

	constraints = sortedConstraints;
}

}
