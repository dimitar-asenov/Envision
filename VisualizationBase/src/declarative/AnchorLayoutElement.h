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

#ifndef VisualizationBase_ANCHORLAYOUTELEMENT_H_
#define VisualizationBase_ANCHORLAYOUTELEMENT_H_

#include "../visualizationbase_api.h"
#include "LayoutElement.h"
#include "AnchorLayoutConstraint.h"

namespace Visualization {

class AnchorLayoutElement : public LayoutElement {
	public:
		enum class PlaceEdge : int {LeftOf, RightOf, CenterOf, VCenterOf, HCenterOf, TopOf, BottomOf};
		enum class AtEdge : int {AtLeftOf, AtRightOf, AtCenterOf, AtVCenterOf, AtHCenterOf, AtTopOf, AtBottomOf};
		enum class FromEdge : int {FromLeftOf, FromRightOf, FromCenterOf, FromVCenterOf, FromHCenterOf, Above, Below};

		// Methods executable on element definition
		AnchorLayoutElement();
		virtual ~AnchorLayoutElement();
		AnchorLayoutElement* put(PlaceEdge placeEdge, Element* placeElement, AtEdge atEdge, Element* fixedElement);
		AnchorLayoutElement* put(PlaceEdge placeEdge, Element* placeElement, int offset, FromEdge fromEdge,
											Element* fixedElement);
		AnchorLayoutElement* put(PlaceEdge placeEdge, Element* placeElement, float relativeEdgePosition,
											Element* fixedElement);

		// Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual void setItemPositions(Item* item, int parentX, int parentY) override;
		virtual void synchronizeWithItem(Item* item) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;

		// Recursive item destruction
		virtual void destroyChildItems(Item* item) override;

	private:
		enum class Edge : int {Left, Right, Center, VCenter, HCenter, Top, Bottom};
		AnchorLayoutElement* put(AnchorLayoutConstraint::Orientation orientation, float relativePlaceEdgePosition,
				Element* placeElement, int offset, float relativeFixedEdgePosition, Element* fixedElement);
		AnchorLayoutConstraint::Orientation orientation(Edge edge);
		AnchorLayoutConstraint::Orientation inferOrientation(Edge firstEdge, Edge secondEdge);
		float relativePosition(Edge edge);
		void addConstraint(QList<AnchorLayoutConstraint*>& constraints, AnchorLayoutConstraint::Orientation orientation,
				float relativePlaceEdgePosition, Element* placeElement, int offset, float relativeFixedEdgePosition,
				Element* fixedElement);
		int placeElements(QList<AnchorLayoutConstraint*>& constraints, AnchorLayoutConstraint::Orientation orientation,
				Item* item);
		void sortConstraints(QList<AnchorLayoutConstraint*>& constraints,
				AnchorLayoutConstraint::Orientation orientation);

		QList<Element*> elementList_{};
		QList<AnchorLayoutConstraint*> horizontalConstraints_{};
		QList<AnchorLayoutConstraint*> verticalConstraints_{};
		bool horizontalHasCircularDependencies_{};
		bool verticalHasCircularDependencies_{};
};

}

#endif /* VisualizationBase_ANCHORLAYOUTELEMENT_H_ */
