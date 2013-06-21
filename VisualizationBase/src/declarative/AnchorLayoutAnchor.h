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

namespace Visualization {

class Item;
class FormElement;

/**
 * An anchor is a constraint on how two elements are positioned relative to each other along one axis.
 * This class holds information about one such constraint.
 * Also, if there are no circular dependencies on the axis to compute, and the fixed element of this class is really
 * already fixed, the method execute can be used to calculate the position of the place element.
 */
class VISUALIZATIONBASE_API AnchorLayoutAnchor {
	public:
		enum class Orientation : int {Auto, Horizontal, Vertical};

		AnchorLayoutAnchor(float relativePlaceEdgePosition, FormElement* placeElement, int offset,
				float relativeFixedEdgePosition, FormElement* fixedElement);
		AnchorLayoutAnchor(const AnchorLayoutAnchor& other) = default; // This is rather complicated and the layout takes
																							// care of doing it properly.
		AnchorLayoutAnchor& operator=(const AnchorLayoutAnchor&) = delete;
		virtual ~AnchorLayoutAnchor();

		virtual AnchorLayoutAnchor* clone() const;

		/**
		 * Returns the position on the \a orientation axis of the element to be placed, assuming its size for \a item was
		 * already calculated, and the position on the orientation axis of the fixed element is already fixed for \a item.
		 */
		int execute(Item* item, Orientation orientation);
		/**
		 * Returns the place element.
		 */
		FormElement* placeElement() const;
		/**
		 * Returns the fixed element.
		 */
		FormElement* fixedElement() const;
		/**
		 * Returns whether this anchor depends on \a other, given the list of all anchors (\a allConstraints).
		 */
		bool dependsOn(AnchorLayoutAnchor* other, QList<AnchorLayoutAnchor*>& allConstraints);
		/**
		 * Returns the relative place edge position.
		 */
		float relativePlaceEdgePosition() const;
		/**
		 * Returns the relative fixed edge position.
		 */
		float relativeFixedEdgePosition() const;
		/**
		 * Returns the offset.
		 */
		int offset() const;

	private:

		// This is used for the adjustment when an AnchorLayout is cloned.
		friend class AnchorLayoutFormElement;
		void setPlaceElement(FormElement* el);
		void setFixedElement(FormElement* el);


		// Do not forget to update the copy constructor if adding new members.
		float relativePlaceEdgePosition_{};
		FormElement* placeElement_{};
		int offset_{};
		float relativeFixedEdgePosition_{};
		FormElement* fixedElement_{};
};

inline FormElement* AnchorLayoutAnchor::placeElement() const
{
	return placeElement_;
}
inline FormElement* AnchorLayoutAnchor::fixedElement() const
{
	return fixedElement_;
}
inline float AnchorLayoutAnchor::relativePlaceEdgePosition() const
{
	return relativePlaceEdgePosition_;
}
inline float AnchorLayoutAnchor::relativeFixedEdgePosition() const
{
	return relativeFixedEdgePosition_;
}
inline int AnchorLayoutAnchor::offset() const
{
	return offset_;
}

inline void AnchorLayoutAnchor::setPlaceElement(FormElement* el) {placeElement_ = el;}
inline void AnchorLayoutAnchor::setFixedElement(FormElement* el) {fixedElement_ = el;}

} /* namespace Visualization */
