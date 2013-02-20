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

AnchorLayoutElement::AnchorLayoutElement()
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

	Orientation o = inferOrientation(edgeToBePlaced, fixedEdge);

	if (o == Orientation::Horizontal) {
		return putX(relativePosition(edgeToBePlaced), placeElement, 0, relativePosition(fixedEdge), fixedElement);
	}
	else {
		return putY(relativePosition(edgeToBePlaced), placeElement, 0, relativePosition(fixedEdge), fixedElement);
	}
}

AnchorLayoutElement* AnchorLayoutElement::put(PlaceEdge placeEdge, Element* placeElement, int offset, FromEdge fromEdge,
		Element* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);
	Edge fixedEdge = static_cast<Edge>(fromEdge);

	Orientation o = inferOrientation(edgeToBePlaced, fixedEdge);

	// compute correct offset
	if (fixedEdge == Edge::Left || fixedEdge == Edge::Top)
		offset = -offset;

	if (o == Orientation::Horizontal) {
		return putX(relativePosition(edgeToBePlaced), placeElement, offset, relativePosition(fixedEdge), fixedElement);
	}
	else {
		return putY(relativePosition(edgeToBePlaced), placeElement, offset, relativePosition(fixedEdge), fixedElement);
	}
}

AnchorLayoutElement* AnchorLayoutElement::put(PlaceEdge placeEdge, Element* placeElement, float relativeEdgePosition,
		Element* fixedElement)
{
	Edge edgeToBePlaced = static_cast<Edge>(placeEdge);

	Orientation o = orientation(edgeToBePlaced);
	Q_ASSERT(o != Orientation::Auto);

	if (o == Orientation::Horizontal) {
		return putX(relativePosition(edgeToBePlaced), placeElement, 0, relativeEdgePosition, fixedElement);
	}
	else {
		return putY(relativePosition(edgeToBePlaced), placeElement, 0, relativeEdgePosition, fixedElement);
	}
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

}
