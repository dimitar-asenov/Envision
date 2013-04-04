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
#include "../layouts/LayoutStyle.h"

namespace Visualization {

class GridLayoutElement : public LayoutElement {
		FLUENT_ELEMENT_INTERFACE(GridLayoutElement);

	public: // Methods executable on element definition
		GridLayoutElement();
		virtual ~GridLayoutElement();
		GridLayoutElement* put(int column, int row, Element* element);
		GridLayoutElement* setSpacing(int spacing);
		GridLayoutElement* setSpacing(int spaceBetweenColumns, int spaceBetweenRows);
		GridLayoutElement* setHorizontalSpacing(int spaceBetweenColumns);
		GridLayoutElement* setVerticalSpacing(int spaceBetweenRows);
		GridLayoutElement* setHorizontalAlignment(LayoutStyle::Alignment horizontalAlignment);
		GridLayoutElement* setVerticalAlignment(LayoutStyle::Alignment verticalAlignment);
		GridLayoutElement* setColumnHorizontalAlignment(int column, LayoutStyle::Alignment horizontalAlignment);
		GridLayoutElement* setRowVerticalAlignment(int row, LayoutStyle::Alignment verticalAlignment);
		GridLayoutElement* setCellHorizontalAlignment(LayoutStyle::Alignment horizontalAlignment);
		GridLayoutElement* setCellVerticalAlignment(LayoutStyle::Alignment verticalAlignment);
		GridLayoutElement* setCellAlignment(LayoutStyle::Alignment horizontalAlignment,
														LayoutStyle::Alignment verticalAlignment);
		GridLayoutElement* setCellSpanning(int columnSpan, int rowSpan);
		GridLayoutElement* setColumnStretchFactor(int column, float stretchFactor);
		GridLayoutElement* setColumnStretchFactors(float stretchFactor);
		GridLayoutElement* setRowStretchFactor(int row, float stretchFactor);
		GridLayoutElement* setRowStretchFactors(float stretchFactor);
		GridLayoutElement* setStretchFactors(float stretchFactor);
		GridLayoutElement* setHasCursorWhenEmpty(bool cursorWhenEmpty);
		GridLayoutElement* setNotLocationEquivalentCursors(bool notLocationEquivalent);
		GridLayoutElement* setNoBoudaryCursors(bool noBoundaryCursors);
		GridLayoutElement* setNoInnerCursors(bool noInnerCursors);

	public: // Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual void setItemPositions(Item* item, int parentX, int parentY) override;
		virtual void synchronizeWithItem(Item* item) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;
		virtual QList<ItemRegion> regions(Item* item, int parentX, int parentY) override;

		int focusedElementIndex(Item* item) const;

	public: // Recursive item destruction
		virtual void destroyChildItems(Item* item) override;

	private:
		int numColumns_{};
		int numRows_{};
		int spaceBetweenColumns_{};
		int spaceBetweenRows_{};

		QPair<int, int> lastCell_{};

		QVector<QVector<Element*>> elementGrid_{};
		QVector<QVector<QPair<int, int>>> spanGrid_{};

		LayoutStyle::Alignment defaultHorizontalAlignment_{};
		LayoutStyle::Alignment defaultVerticalAlignment_{};
		QVector<LayoutStyle::Alignment> defaultRowVerticalAlignments_{};
		QVector<LayoutStyle::Alignment> defaultColumnHorizontalAlignments_{};
		QVector<QVector<LayoutStyle::Alignment>> cellHorizontalAlignmentGrid_{};
		QVector<QVector<LayoutStyle::Alignment>> cellVerticalAlignmentGrid_{};

		float defaultColumnStretchFactor_{};
		float defaultRowStretchFactor_{};
		QVector<float> columnStretchFactors_{};
		QVector<float> rowStretchFactors_{};
		float overallColumnStretchFactor_{};
		float overallRowStretchFactor_{};

		bool hasCursorWhenEmpty_{false};
		bool notLocationEquivalentCursors_{false};
		bool noBoundaryCursors_{false};
		bool noInnerCursors_{false};

		void computeOverallStretchFactors();
		void adjustSize(int containColumn, int containRow);
		void adjustCursorRegionToAvoidZeroSize(QRect& region, bool horizontal, bool first, bool last);
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
	defaultHorizontalAlignment_ = horizontalAlignment;
	for (int x = 0; x < numColumns_; x++)
	{
		defaultColumnHorizontalAlignments_[x] = horizontalAlignment;
		for (int y = 0; y < numRows_; y++)
			cellHorizontalAlignmentGrid_[x][y] = horizontalAlignment;
	}

	return this;
}
inline GridLayoutElement* GridLayoutElement::setVerticalAlignment(LayoutStyle::Alignment verticalAlignment)
{
	defaultVerticalAlignment_ = verticalAlignment;
	for (int y = 0; y < numRows_; y++)
	{
		defaultRowVerticalAlignments_[y] = verticalAlignment;
		for (int x = 0; x < numColumns_; x++)
			cellVerticalAlignmentGrid_[x][y] = verticalAlignment;
	}
	return this;
}
inline GridLayoutElement* GridLayoutElement::setColumnHorizontalAlignment(int column,
		LayoutStyle::Alignment horizontalAlignment)
{
	adjustSize(column, 0);

	defaultColumnHorizontalAlignments_[column] = horizontalAlignment;
	for (int y = 0; y < numRows_; y++)
		cellHorizontalAlignmentGrid_[column][y] = horizontalAlignment;

	return this;
}
inline GridLayoutElement* GridLayoutElement::setRowVerticalAlignment(int row, LayoutStyle::Alignment verticalAlignment)
{
	adjustSize(0, row);

	defaultRowVerticalAlignments_[row] = verticalAlignment;
	for (int x = 0; x < numColumns_; x++)
		cellVerticalAlignmentGrid_[x][row] = verticalAlignment;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setCellHorizontalAlignment(LayoutStyle::Alignment horizontalAlignment)
{
	int column = lastCell_.first;
	int row = lastCell_.second;
	cellHorizontalAlignmentGrid_[column][row] = horizontalAlignment;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setCellVerticalAlignment(LayoutStyle::Alignment verticalAlignment)
{
	int column = lastCell_.first;
	int row = lastCell_.second;
	cellVerticalAlignmentGrid_[column][row] = verticalAlignment;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setCellAlignment(LayoutStyle::Alignment horizontalAlignment,
																					LayoutStyle::Alignment verticalAlignment)
{
	int column = lastCell_.first;
	int row = lastCell_.second;
	cellHorizontalAlignmentGrid_[column][row] = horizontalAlignment;
	cellVerticalAlignmentGrid_[column][row] = verticalAlignment;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setCellSpanning(int columnSpan, int rowSpan)
{
	int column = lastCell_.first;
	int row = lastCell_.second;
	adjustSize(column + columnSpan - 1, row + rowSpan - 1);
	spanGrid_[column][row] = QPair<int, int>(columnSpan, rowSpan);
	return this;
}
inline GridLayoutElement* GridLayoutElement::setColumnStretchFactor(int column, float stretchFactor)
{
	adjustSize(column, 0);
	columnStretchFactors_[column] = stretchFactor;
	computeOverallStretchFactors();
	return this;
}
inline GridLayoutElement* GridLayoutElement::setColumnStretchFactors(float stretchFactor)
{
	defaultColumnStretchFactor_ = stretchFactor;
	columnStretchFactors_ = QVector<float>(numColumns_, stretchFactor);
	computeOverallStretchFactors();
	return this;
}
inline GridLayoutElement* GridLayoutElement::setRowStretchFactor(int row, float stretchFactor)
{
	adjustSize(0, row);
	rowStretchFactors_[row] = stretchFactor;
	computeOverallStretchFactors();
	return this;
}
inline GridLayoutElement* GridLayoutElement::setRowStretchFactors(float stretchFactor)
{
	defaultRowStretchFactor_ = stretchFactor;
	rowStretchFactors_ = QVector<float>(numRows_, stretchFactor);
	computeOverallStretchFactors();
	return this;
}
inline GridLayoutElement* GridLayoutElement::setStretchFactors(float stretchFactor)
{
	defaultColumnStretchFactor_ = stretchFactor;
	defaultRowStretchFactor_ = stretchFactor;
	columnStretchFactors_ = QVector<float>(numColumns_, stretchFactor);
	rowStretchFactors_ = QVector<float>(numRows_, stretchFactor);
	computeOverallStretchFactors();
	return this;
}
inline GridLayoutElement* GridLayoutElement::setHasCursorWhenEmpty(bool cursorWhenEmpty)
{
	hasCursorWhenEmpty_ = cursorWhenEmpty;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setNotLocationEquivalentCursors(
		bool notLocationEquivalentCursors)
{
	notLocationEquivalentCursors_ = notLocationEquivalentCursors;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setNoBoudaryCursors(bool noBoundaryCursors)
{
	noBoundaryCursors_ = noBoundaryCursors;
	return this;
}
inline GridLayoutElement* GridLayoutElement::setNoInnerCursors(bool noInnerCursors)
{
	noInnerCursors_ = noInnerCursors;
	return this;
}

} /* namespace Visualization */

#endif /* VisualizationBase_GRIDLAYOUTELEMENT_H_ */
