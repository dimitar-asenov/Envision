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

#include "AnchorLayoutFormElement.h"

#include "AnchorLayoutConstraintSolver.h"
#include "../items/ItemRegion.h"

namespace Visualization {

AnchorLayoutFormElement::AnchorLayoutFormElement()
: horizontalConstraints_{QList<AnchorLayoutAnchor*>()}, verticalConstraints_{QList<AnchorLayoutAnchor*>()},
  horizontalNeedsConstraintSolver_{false}, verticalNeedsConstraintSolver_{false},
  solver_{new AnchorLayoutConstraintSolver()}
{}

AnchorLayoutFormElement::~AnchorLayoutFormElement()
{
	// elements were deleted by Element
	SAFE_DELETE(solver_);
}

AnchorLayoutFormElement* AnchorLayoutFormElement::put(PlaceEdge placeEdge, FormElement* placeElement, AtEdge atEdge,
		FormElement* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);
	Edge fixedEdge = static_cast<Edge>(atEdge);

	AnchorLayoutAnchor::Orientation orientation = inferOrientation(edgeToBePlaced, fixedEdge);

	return put(orientation, relativePosition(edgeToBePlaced), placeElement, 0, relativePosition(fixedEdge),
					fixedElement);
}

AnchorLayoutFormElement* AnchorLayoutFormElement::put(PlaceEdge placeEdge, FormElement* placeElement, int offset, FromEdge fromEdge,
		FormElement* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);
	Edge fixedEdge = static_cast<Edge>(fromEdge);

	AnchorLayoutAnchor::Orientation orientation = inferOrientation(edgeToBePlaced, fixedEdge);

	// compute correct offset
	if (fixedEdge == Edge::Left || fixedEdge == Edge::Top) offset = -offset;

	return put(orientation, relativePosition(edgeToBePlaced), placeElement, offset, relativePosition(fixedEdge),
					fixedElement);
}

AnchorLayoutFormElement* AnchorLayoutFormElement::put(PlaceEdge placeEdge, FormElement* placeElement, float relativeEdgePosition,
		FormElement* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);

	AnchorLayoutAnchor::Orientation orientation = this->orientation(edgeToBePlaced);
	Q_ASSERT(orientation != AnchorLayoutAnchor::Orientation::Auto);

	return put(orientation, relativePosition(edgeToBePlaced), placeElement, 0, relativeEdgePosition, fixedElement);
}

void AnchorLayoutFormElement::computeSize(Item* item, int /*availableWidth*/, int /*availableHeight*/)
{
	// compute size of each sub-element and set their position to (0, 0)
	for (FormElement* element : children())
	{
		element->computeSize(item, 0, 0);
		element->setPos(item, QPoint(0, 0));
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
	for (FormElement* element : children())
	{
		element->setPos(item, QPoint(element->x(item) + adjustmentX, element->y(item) + adjustmentY));
		if (element->xEnd(item) > maxX) maxX = element->xEnd(item);
		if (element->yEnd(item) > maxY) maxY = element->yEnd(item);
	}
	setSize(item, QSize(maxX + rightMargin(), maxY + bottomMargin()));
}

bool AnchorLayoutFormElement::sizeDependsOnParent(const Item* /*item*/) const
{
	return false;
}

AnchorLayoutFormElement* AnchorLayoutFormElement::put(AnchorLayoutAnchor::Orientation orientation,
		float relativePlaceEdgePosition, FormElement* placeElement, int offset, float relativeFixedEdgePosition,
		FormElement* fixedElement)
{
	Q_ASSERT(orientation != AnchorLayoutAnchor::Orientation::Auto);

	if (!children().contains(placeElement))
		addChild(placeElement);

	if (!children().contains(fixedElement))
		addChild(fixedElement);

	if (orientation == AnchorLayoutAnchor::Orientation::Horizontal)
		addConstraint(horizontalConstraints_, orientation, relativePlaceEdgePosition, placeElement, offset,
																			relativeFixedEdgePosition, fixedElement);
	else // orientation == AnchorLayoutConstraint::Orientation::Vertical
		addConstraint(verticalConstraints_, orientation, relativePlaceEdgePosition, placeElement, offset,
																			relativeFixedEdgePosition, fixedElement);
	return this;
}

AnchorLayoutAnchor::Orientation AnchorLayoutFormElement::orientation(Edge edge)
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

AnchorLayoutAnchor::Orientation AnchorLayoutFormElement::inferOrientation(Edge firstEdge, Edge secondEdge)
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

float AnchorLayoutFormElement::relativePosition(Edge edge)
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

void AnchorLayoutFormElement::addConstraint(QList<AnchorLayoutAnchor*>& constraints,
		AnchorLayoutAnchor::Orientation orientation, float relativePlaceEdgePosition, FormElement* placeElement,
		int offset, float relativeFixedEdgePosition, FormElement* fixedElement)
{
	constraints.append(new AnchorLayoutAnchor(relativePlaceEdgePosition, placeElement, offset,
																	relativeFixedEdgePosition, fixedElement));
	sortConstraints(constraints, orientation);
}

int AnchorLayoutFormElement::placeElements(QList<AnchorLayoutAnchor*>& constraints,
		AnchorLayoutAnchor::Orientation orientation, Item* item)
{
	Q_ASSERT(orientation != AnchorLayoutAnchor::Orientation::Auto);
	bool axisNeedsConstraintSolver = (orientation == AnchorLayoutAnchor::Orientation::Horizontal
														&& horizontalNeedsConstraintSolver_)
												|| (orientation == AnchorLayoutAnchor::Orientation::Vertical
														&& verticalNeedsConstraintSolver_);

	int minPos = 0;
	if (axisNeedsConstraintSolver)
		solver_->placeElements(children(), constraints, orientation, item);
	else
		for (auto c : constraints)
		{
			int pos = c->execute(item, orientation);
			if (pos < minPos)
				minPos = pos;
		}
	return minPos;
}

void AnchorLayoutFormElement::sortConstraints(QList<AnchorLayoutAnchor*>& constraints,
		AnchorLayoutAnchor::Orientation orientation)
{
	// check if this orientation does not already have circular dependencies (if so, don't sort)
	Q_ASSERT(orientation != AnchorLayoutAnchor::Orientation::Auto);
	if (orientation == AnchorLayoutAnchor::Orientation::Horizontal && horizontalNeedsConstraintSolver_)
		return;
	else if (orientation == AnchorLayoutAnchor::Orientation::Vertical && verticalNeedsConstraintSolver_)
		return;

	QList<AnchorLayoutAnchor*> sortedConstraints;
	QList<FormElement*> elementQueue;

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
			horizontalNeedsConstraintSolver_ = true;
		else // orientation == AnchorLayoutConstraint::Orientation::Vertical
			verticalNeedsConstraintSolver_ = true;
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
							horizontalNeedsConstraintSolver_ = true;
						else // orientation == AnchorLayoutConstraint::Orientation::Vertical
							verticalNeedsConstraintSolver_ = true;
						return;
					}
				}
				else elementQueue.append(c->placeElement());
			}
		}
	}

	constraints = sortedConstraints;

	// check if element is not already assumed to be fixed/placed
	bool needsConstraintSolver = false;
	for (int i=0; i<constraints.size(); ++i)
	{
		FormElement* lastPlaceElement = constraints.at(i)->placeElement();
		for (int j = 0; j < i; ++j)
		{
			if (constraints.at(j)->fixedElement() == lastPlaceElement)
				needsConstraintSolver = true;
			if (constraints.at(j)->placeElement() == lastPlaceElement)
				needsConstraintSolver = true;
		}
	}
	if (needsConstraintSolver)
	{
		if (orientation == AnchorLayoutAnchor::Orientation::Horizontal)
			horizontalNeedsConstraintSolver_ = true;
		else // orientation == AnchorLayoutAnchor::Orientation::Vertical
			verticalNeedsConstraintSolver_ = true;
	}
}

}
