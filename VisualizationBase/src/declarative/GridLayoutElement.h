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

#ifndef VisualizationBase_GRIDLAYOUTELEMENT_H_
#define VisualizationBase_GRIDLAYOUTELEMENT_H_

#include "../visualizationbase_api.h"
#include "LayoutElement.h"
#include "../layouts/LayoutStyle.h" // TODO: is it OK to import the alignments form here or not?

namespace Visualization {

class GridLayoutElement : public LayoutElement {
		FLUENT_ELEMENT_INTERFACE(GridLayoutElement);

	public: // Methods executable on element definition
		GridLayoutElement(int numColumns, int numRows);
		virtual ~GridLayoutElement();
		GridLayoutElement* addElement(Element* element, int column, int row, int columnSpan=1, int rowSpan=1);
		GridLayoutElement* setSpacing(int spacing);
		GridLayoutElement* setSpacing(int spaceBetweenColumns, int spaceBetweenRows);
		GridLayoutElement* setHorizontalSpacing(int spaceBetweenColumns);
		GridLayoutElement* setVerticalSpacing(int spaceBetweenRows);
		GridLayoutElement* setHorizontalAlignment(LayoutStyle::Alignment horizontalAlignment);
		GridLayoutElement* setVerticalAlignment(LayoutStyle::Alignment verticalAlignment);
		GridLayoutElement* setColumnStretchFactor(int column, float stretchFactor);
		GridLayoutElement* setColumnStretchFactors(float stretchFactor);
		GridLayoutElement* setRowStretchFactor(int row, float stretchFactor);
		GridLayoutElement* setRowStretchFactors(float stretchFactor);
		GridLayoutElement* setStretchFactors(float stretchFactor);

	public: // Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual void setItemPositions(Item* item, int parentX, int parentY) override;
		virtual void synchronizeWithItem(Item* item) override;

	public: // Recursive item destruction
		virtual void destroyChildItems(Item* item) override;

	private:
		int numColumns_{};
		int numRows_{};
		int spaceBetweenColumns_{};
		int spaceBetweenRows_{};
		LayoutStyle::Alignment horizontalAlignment_{};
		LayoutStyle::Alignment verticalAlignment_{};
		QVector<QVector<Element*>> elementGrid_{};
		QVector<QVector<QPair<int, int>>> spanGrid_{};
		QVector<float> columnStretchFactors_{};
		QVector<float> rowStretchFactors_{};
};

inline GridLayoutElement* GridLayoutElement::setSpacing(int spacing)
{
	spaceBetweenRows_ = spacing;
	spaceBetweenColumns_ = spacing;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setSpacing(int spaceBetweenColumns, int spaceBetweenRows)
{
	spaceBetweenColumns_ = spaceBetweenColumns;
	spaceBetweenRows_ = spaceBetweenRows;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setHorizontalSpacing(int spaceBetweenColumns)
{
	spaceBetweenColumns_ = spaceBetweenColumns;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setVerticalSpacing(int spaceBetweenRows)
{
	spaceBetweenRows_ = spaceBetweenRows;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setHorizontalAlignment(LayoutStyle::Alignment horizontalAlignment)
{
	horizontalAlignment_ = horizontalAlignment;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setVerticalAlignment(LayoutStyle::Alignment verticalAlignment)
{
	verticalAlignment_ = verticalAlignment;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setColumnStretchFactor(int column, float stretchFactor)
{
	columnStretchFactors_[column] = stretchFactor;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setColumnStretchFactors(float stretchFactor)
{
	columnStretchFactors_ = QVector<float>(numColumns_, stretchFactor);
	return this;
}
inline GridLayoutElement* GridLayoutElement::setRowStretchFactor(int row, float stretchFactor)
{
	rowStretchFactors_[row] = stretchFactor;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setRowStretchFactors(float stretchFactor)
{
	rowStretchFactors_ = QVector<float>(numRows_, stretchFactor);
	return this;
}
inline GridLayoutElement* GridLayoutElement::setStretchFactors(float stretchFactor)
{
	columnStretchFactors_ = QVector<float>(numColumns_, stretchFactor);
	rowStretchFactors_ = QVector<float>(numRows_, stretchFactor);
	return this;
}

} /* namespace Visualization */

#endif /* VisualizationBase_GRIDLAYOUTELEMENT_H_ */
