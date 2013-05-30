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

#pragma once

#include "../visualizationbase_api.h"
#include "LayoutFormElement.h"
#include "AnchorLayoutAnchor.h"
#include "Enumerations.h"

using namespace DeclarativeEnumerations;
namespace Visualization {

class AnchorLayoutConstraintSolver;

class VISUALIZATIONBASE_API AnchorLayoutFormElement : public LayoutFormElement {
		FLUENT_ELEMENT_INTERFACE(AnchorLayoutFormElement);

	public:
		AnchorLayoutFormElement();
		virtual ~AnchorLayoutFormElement();

		// Methods executable on element definition
		/**
		 * Add an anchor to the layout, placing the \a placeEdge of the \a placeElement at the \a atEdge of the
		 * \a fixedElement.
		 * Returns a pointer to this AnchorLayoutElement.
		 */
		AnchorLayoutFormElement* put(PlaceEdge placeEdge, FormElement* placeElement, AtEdge atEdge, FormElement* fixedElement);
		/**
		 * Add an anchor to the layout, placing the \a placeEdge of the \a placeElement \a offset units from the
		 * \a fromEdge of the \a fixedElement.
		 * Returns a pointer to this AnchorLayoutElement.
		 */
		AnchorLayoutFormElement* put(PlaceEdge placeEdge, FormElement* placeElement, int offset, FromEdge fromEdge,
											FormElement* fixedElement);
		/**
		 * Add an anchor to the layout, placing the \a placeEdge of the \a placeElement at the position
		 * \a fixedElement.position + \a fixedElement.size * \a relativeEdge in the axis given by the \a placeEdge.
		 * Returns a pointer to this AnchorLayoutElement.
		 */
		AnchorLayoutFormElement* put(PlaceEdge placeEdge, FormElement* placeElement, float relativeEdgePosition,
											FormElement* fixedElement);

		// Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;

	private:
		enum Edge {Left, Right, Center, VCenter, HCenter, Top, Bottom};
		/**
		 * General method, that all the other put methods get translated to. It puts an anchor in the \a orientation,
		 * and places the \a relativePlaceEdgePosition of \a placeElement \a offset from the \a relativeFixedEdgePositon
		 * of the \a fixedElement.
		 * Returns a pointer to this AnchorLayoutElement.
		 */
		AnchorLayoutFormElement* put(AnchorLayoutAnchor::Orientation orientation, float relativePlaceEdgePosition,
				FormElement* placeElement, int offset, float relativeFixedEdgePosition, FormElement* fixedElement);
		/**
		 * Returns the orientation of the given \a edge. (E.g. horizontal for left, vertical for top, ...)
		 * If the orientation is not given for this \a edge (i.e. it is center), returns Orientation::Auto.
		 */
		AnchorLayoutAnchor::Orientation orientation(Edge edge);
		/**
		 * Computes orientation of \a firstEdge and \a secondEdge, fails if orientation cannot be inferred (both are
		 * Center) of if the edges have conflicting orientations (e.g. Orientation::Left and Orientation::Top).
		 * Returns either Orientation::Horizontal or Orientation::Vertical.
		 */
		AnchorLayoutAnchor::Orientation inferOrientation(Edge firstEdge, Edge secondEdge);

		/**
		 * Returns the relative position in an element of this \a edge. (0 for left, 0.5 for center, 1 for right, ...)
		 */
		float relativePosition(Edge edge);
		/**
		 * Adds a constraint, consisting of the \a orientation, the \a relativePlaceEdgePosition, the \a placeElement,
		 * the \a offset, the \a relativeFixedEdgePosition, and the \a fixedElement to the list of \a constraints.
		 */
		void addConstraint(QList<AnchorLayoutAnchor*>& constraints, AnchorLayoutAnchor::Orientation orientation,
				float relativePlaceEdgePosition, FormElement* placeElement, int offset, float relativeFixedEdgePosition,
				FormElement* fixedElement);
		/**
		 * Computes the positions in the \a orientation of all the elements for this \a item, using the list of
		 * \a constraints.
		 */
		int placeElements(QList<AnchorLayoutAnchor*>& constraints, AnchorLayoutAnchor::Orientation orientation,
				Item* item);
		/**
		 * Sorts the constraints by what needs to be done first. Also detects if there is a circular dependency, and the
		 * linear programming solver needs to be used.
		 */
		void sortConstraints(QList<AnchorLayoutAnchor*>& constraints,
				AnchorLayoutAnchor::Orientation orientation);

		QList<AnchorLayoutAnchor*> horizontalConstraints_{};
		QList<AnchorLayoutAnchor*> verticalConstraints_{};
		bool horizontalNeedsConstraintSolver_{};
		bool verticalNeedsConstraintSolver_{};

		AnchorLayoutConstraintSolver* solver_{};
};

} /* namespace Visualization */
